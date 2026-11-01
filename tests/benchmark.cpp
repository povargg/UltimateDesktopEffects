// CPU-only comparison of the unified dispatcher against direct calls to the same engines.
#include "../src/effects.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
static void Direct(Effects&e,float dt,const World&w){switch(e.Active()){case 0:e.snow.Update(dt,w,{0,0},false);break;case 1:e.rain.Update(dt,w,{0,0},false);break;case 2:e.leaves.Update(dt,w,{0,0},false);break;case 3:e.halloween.Update(dt,w,{0,0},false);break;}}
static double Run(int effect,bool dispatch){
 ResetAllProfiles();SuiteCfg().effect=effect;RngState()=1234567;
 World w;w.virt={0,0,1920,1080};w.monitors.push_back({w.virt,{0,0,1920,1040},1040,1});w.wins.push_back({(HWND)1,{120,300,1050,850},false,false});
 Effects e;e.Sync(w);auto start=std::chrono::steady_clock::now();
 for(int n=0;n<3600;++n){if(dispatch)e.Update(1.f/60,w,{0,0},false);else Direct(e,1.f/60,w);}
 auto end=std::chrono::steady_clock::now();return std::chrono::duration<double,std::milli>(end-start).count();
}
int main(){std::printf("CPU-only; 3600 simulated frames per run; median of 3; native Linux; no GPU or Windows UI\n");for(int effect=0;effect<4;++effect){double direct[3],suite[3];for(int n=0;n<3;++n){if(n%2){suite[n]=Run(effect,true);direct[n]=Run(effect,false);}else{direct[n]=Run(effect,false);suite[n]=Run(effect,true);}}std::sort(direct,direct+3);std::sort(suite,suite+3);std::printf("%s direct_ms=%.3f unified_ms=%.3f ratio=%.3f per_frame_unified_ms=%.6f\n",ProfileName(effect),direct[1],suite[1],suite[1]/direct[1],suite[1]/3600);}return 0;}
