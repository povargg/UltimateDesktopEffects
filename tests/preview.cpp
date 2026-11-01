#include "../src/effects.h"
#include <fstream>
#include <iomanip>
#include <type_traits>
template<class Surfaces>static void SurfacesJson(std::ofstream&o,const Surfaces& surfaces){bool comma=false;for(auto&s:surfaces){if(comma)o<<',';comma=true;o<<"{\"left\":"<<s.left<<",\"top\":"<<s.top<<",\"width\":"<<s.width<<",\"h\":[";for(size_t i=0;i<s.h.size();++i){if(i)o<<',';o<<s.h[i];}o<<"],\"objects\":[";bool p=false;
 if constexpr(std::is_same_v<Surfaces,std::vector<leavesfx::Surface>>){for(auto&l:s.leaves){if(p)o<<',';p=true;o<<'['<<l.x<<','<<l.size<<','<<l.base<<','<<l.rotation<<",0,"<<(int)l.variant<<']';}}
 if constexpr(std::is_same_v<Surfaces,std::vector<halloweenfx::Surface>>){for(auto&l:s.objects){if(p)o<<',';p=true;o<<'['<<l.x<<','<<l.size<<','<<l.base<<','<<l.rotation<<','<<(int)l.family<<','<<(int)l.variant<<']';}}
 o<<"]}";}}
int main(){
 World w;w.virt={0,0,1280,800};w.monitors.push_back({w.virt,{0,0,1280,758},758,1});w.wins.push_back({(HWND)1,{120,240,640,640},false,false});w.wins.push_back({(HWND)2,{720,400,1200,700},false,false});
 ResetAllProfiles();snowfx::Cfg().intensity=rainfx::Cfg().intensity=leavesfx::Cfg().intensity=halloweenfx::Cfg().intensity=75;
 snowfx::Cfg().cursorWind=rainfx::Cfg().cursorWind=leavesfx::Cfg().cursorWind=halloweenfx::Cfg().cursorWind=0;
 halloweenfx::Cfg().spiderInterval=5;Effects e;e.Sync(w);
 for(int effect=0;effect<4;++effect){e.Select(effect,w);for(int i=0;i<9000;++i){e.Update(1.f/60,w,{0,0},false);if(i%15==0)e.Sync(w);}
  std::ofstream o(std::string("build/preview-")+ProfileName(effect)+".json");o<<std::setprecision(7)<<"{\"effect\":"<<effect<<",\"surfaces\":[";
  switch(effect){case 0:SurfacesJson(o,e.snow.surfaces);break;case 1:SurfacesJson(o,e.rain.surfaces);break;case 2:SurfacesJson(o,e.leaves.surfaces);break;case 3:SurfacesJson(o,e.halloween.surfaces);break;}
  o<<"],\"airborne\":[";bool p=false;
  if(effect==0)for(auto&f:e.snow.flakes){if(p)o<<',';p=true;o<<'['<<f.x<<','<<f.y<<','<<f.size<<','<<f.rot<<",0,"<<(int)f.variant<<','<<(int)f.kind<<']';}
  if(effect==1)for(auto&f:e.rain.drops){if(p)o<<',';p=true;o<<'['<<f.x<<','<<f.y<<','<<f.size<<",0,0,0,"<<(int)f.kind<<']';}
  if(effect==2)for(auto&f:e.leaves.airborne){if(p)o<<',';p=true;o<<'['<<f.x<<','<<f.y<<','<<f.size<<','<<f.rotation<<",0,"<<(int)f.variant<<','<<f.phase<<']';}
  if(effect==3)for(auto&f:e.halloween.airborne){if(p)o<<',';p=true;o<<'['<<f.x<<','<<f.y<<','<<f.size<<','<<f.rotation<<','<<(int)f.family<<','<<(int)f.variant<<','<<f.phase<<']';}
  o<<"],\"webs\":[";p=false;if(effect==3)for(auto&s:e.halloween.webs){if(p)o<<',';p=true;auto t=halloweenfx::Scene::Thread(s);o<<'['<<s.x<<','<<s.y<<','<<s.size<<','<<(int)s.variant<<','<<(int)s.state<<','<<t.ax<<','<<t.ay<<','<<t.bx<<','<<t.by<<']';}
  o<<"]}";
 }
}
