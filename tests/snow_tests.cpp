#include "../src/effects/snow/snow.h"
#include <cstdio>
#include <functional>
#include <numeric>
#include <stdexcept>
using namespace snowfx;
static Settings config;Settings& snowfx::Cfg(){return config;}
static void Check(bool b,const char* why){if(!b)throw std::runtime_error(why);}
static void Reset(){config=Settings();config.intensity=0;config.gusts=0;config.cursorWind=0;config.angle=0;RngState()=1234567;}
static World Desktop(int width=1920,int height=1080,float scale=1){World w;w.virt={0,0,width,height};w.monitors.push_back({w.virt,{0,0,width,height-(int)(40*scale)},height-(int)(40*scale),scale});return w;}
static void Window(World&w,uintptr_t key,RectI r,bool transient=false){w.wins.push_back({(HWND)key,r,false,transient});}
static Surface& Find(Snow&s,uintptr_t key){for(auto&edge:s.surfaces)if(edge.key==key)return edge;throw std::runtime_error("missing surface");}
static void Step(Snow&s,World&w,int n,float dt=1.f/60){for(int i=0;i<n;++i)s.Update(dt,w,{0,0},false);}
static float Volume(const Surface&s){return std::accumulate(s.h.begin(),s.h.end(),0.f)*kCell;}
static Flake Falling(float x,float y){Flake f{};f.x=x;f.y=y;f.vy=900;f.size=5;f.z=1;f.mass=20;f.s=1;f.alpha=1;f.kind=FLAKE_CHUNK;f.freq=1;return f;}
static void Finite(const Snow&s){Check(s.flakes.size()<=7000,"particle population unbounded");for(auto f:s.flakes)Check(std::isfinite(f.x)&&std::isfinite(f.y)&&std::isfinite(f.vx)&&std::isfinite(f.vy),"invalid particle");for(auto&edge:s.surfaces){Check(edge.h.size()==(size_t)edge.cells,"height-grid mismatch");for(auto h:edge.h)Check(std::isfinite(h)&&h>=0,"invalid drift");}}
int main(){int pass=0,fail=0;auto test=[&](const char* title,std::function<void()> fn){Reset();try{fn();++pass;std::printf("PASS %s\n",title);}catch(const std::exception&e){++fail;std::printf("FAIL %s: %s\n",title,e.what());}};
 test("snow has all four crystal shapes",[]{Cfg().intensity=100;Cfg().crystalShare=100;auto w=Desktop();Snow s;s.Sync(w);Step(s,w,1);bool seen[4]={};for(auto f:s.flakes){Check(f.kind==FLAKE_CRYSTAL&&f.variant<4,"crystal identity");seen[f.variant]=true;}for(bool v:seen)Check(v,"missing crystal");});
 test("zero crystal share uses only soft flakes",[]{Cfg().intensity=60;Cfg().crystalShare=0;auto w=Desktop();Snow s;s.Sync(w);Step(s,w,1);Check(!s.flakes.empty(),"no snow");for(auto f:s.flakes)Check(f.kind==FLAKE_DOT,"unexpected crystal");});
 test("wind deflects falling snow",[]{Cfg().angle=35;auto w=Desktop();Snow s;s.Sync(w);auto f=Falling(500,100);f.kind=FLAKE_DOT;f.vy=80;s.flakes.push_back(f);Step(s,w,120);Check(s.flakes[0].x>520&&s.flakes[0].y>180,"no wind response");});
 test("crystal rotation advances",[]{auto w=Desktop();Snow s;auto f=Falling(500,100);f.kind=FLAKE_CRYSTAL;f.vy=80;f.vrot=1;s.flakes.push_back(f);Step(s,w,60);Check(s.flakes[0].rot>.9f,"no rotation");});
 test("flake size slider changes emitted size",[]{Cfg().intensity=60;auto w=Desktop();Snow a,b;Cfg().flakeSize=0;RngState()=4;Step(a,w,1);Cfg().flakeSize=100;RngState()=4;Step(b,w,1);Check(b.flakes[0].size>a.flakes[0].size*1.5f,"size ignored");});
 test("snow clump lands on a window edge",[]{auto w=Desktop();Window(w,1,{100,300,900,800});Snow s;s.Sync(w);s.flakes.push_back(Falling(400,295));Step(s,w,1);Check(Volume(Find(s,1))>0&&s.flakes.empty(),"window landing failed");});
 test("snow clump lands above the taskbar",[]{auto w=Desktop();Snow s;s.Sync(w);s.flakes.push_back(Falling(500,1035));Step(s,w,1);Check(Volume(s.surfaces[0])>0,"ground landing failed");});
 test("snow uses the first crossed window at 15 FPS",[]{auto w=Desktop();Window(w,1,{100,300,900,500});Window(w,2,{100,500,900,900});Snow s;s.Sync(w);s.flakes.push_back(Falling(400,270));Step(s,w,1,1.f/15);Check(Volume(Find(s,1))>0&&Volume(Find(s,2))==0,"wrong landing edge");});
 test("zero accumulation adds no ordinary snowfall mass",[]{auto w=Desktop();Snow s;s.Sync(w);auto f=Falling(400,1039);f.kind=FLAKE_DOT;f.vy=200;s.flakes.push_back(f);Cfg().accumulation=0;Step(s,w,1);Check(Volume(s.surfaces[0])==0,"zero accumulation grew drift");});
 test("snow maximum-density stress remains finite",[]{Cfg().intensity=100;Cfg().fallSpeed=100;Cfg().gusts=100;Cfg().angle=60;auto w=Desktop();Window(w,1,{100,300,1600,900});Snow s;s.Sync(w);for(int i=0;i<1800;++i){s.Update(i%2?1.f/15:1.f/144,w,{i%1900,900},true);if(i%60==0)Finite(s);}Finite(s);});
 test("melting removes an old snow layer",[]{Cfg().melt=100;auto w=Desktop();Snow s;s.Sync(w);std::fill(s.surfaces[0].h.begin(),s.surfaces[0].h.end(),3);Step(s,w,600);Check(Volume(s.surfaces[0])<.01f,"snow did not melt");});
 test("hidden falling snow can still accumulate",[]{Cfg().showFalling=false;auto w=Desktop();Snow s;s.Sync(w);auto f=Falling(500,1039);f.kind=FLAKE_DOT;f.vy=200;s.flakes.push_back(f);Check(!s.VisibleMotion(),"hidden dots reported visible");Step(s,w,1);Check(Volume(s.surfaces[0])>0,"hidden snow failed to accumulate");});
 test("foreground windows occlude back-window drifts",[]{auto w=Desktop();Window(w,1,{250,200,650,800});Window(w,2,{100,350,900,850});Snow s;s.Sync(w);auto&e=Find(s,2);int c=(400-e.left)/kCell;Check(e.hideBelow[c]>=150,"back drift not occluded");});
 test("slow window movement preserves snow mass",[]{auto w=Desktop();Window(w,1,{100,300,900,800});Snow s;s.Sync(w);Step(s,w,60);auto&e=Find(s,1);std::fill(e.h.begin(),e.h.end(),4);float volume=Volume(e);w.wins[0].rc={150,300,950,800};s.Sync(w);Check(std::fabs(Volume(Find(s,1))-volume)<.01f,"slow drag lost mass");});
 test("window resizing crops and releases overhanging snow",[]{auto w=Desktop();Window(w,1,{100,300,900,800});Snow s;s.Sync(w);std::fill(Find(s,1).h.begin(),Find(s,1).h.end(),5);w.wins[0].rc={100,300,500,800};s.Sync(w);Check(Find(s,1).width<400&&!s.flakes.empty(),"resize failed to spill snow");Finite(s);});
 test("closing a window removes its drift",[]{auto w=Desktop();Window(w,1,{100,300,900,800});Snow s;s.Sync(w);w.wins.clear();s.Sync(w);Check(s.surfaces.size()==1&&s.surfaces[0].ground,"closed window kept a drift");});
 test("shake releases snow and empties drifts",[]{auto w=Desktop();Snow s;s.Sync(w);std::fill(s.surfaces[0].h.begin(),s.surfaces[0].h.end(),4);s.ShakeAll();Check(!s.flakes.empty()&&Volume(s.surfaces[0])==0,"shake failed");Finite(s);});
 test("cursor brushes snow away",[]{auto w=Desktop();Snow s;s.Sync(w);std::fill(s.surfaces[0].h.begin(),s.surfaces[0].h.end(),8);float before=Volume(s.surfaces[0]);s.Update(1.f/60,w,{500,1035},true);Check(Volume(s.surfaces[0])<before,"cursor did not knock snow");});
 test("taskbar and window switches create independent surfaces",[]{auto w=Desktop();Window(w,1,{100,300,900,800});Snow s;Cfg().onTaskbar=false;s.Sync(w);Check(s.surfaces.size()==1&&!s.surfaces[0].ground,"taskbar switch ignored");Cfg().onTaskbar=true;Cfg().onWindows=false;s.Sync(w);Check(s.surfaces.size()==1&&s.surfaces[0].ground,"window switch ignored");});
 test("mixed DPI uses monitor-scaled snowflakes",[]{Cfg().intensity=100;auto w=Desktop(5760,2160);w.monitors[0].rc={0,0,1920,1080};w.monitors[0].groundY=1040;w.monitors.push_back({{1920,0,5760,2160},{1920,0,5760,2080},2080,2});Snow s;s.Sync(w);Step(s,w,1);float sums[2]={};int count[2]={};for(auto f:s.flakes){int i=f.s>1.5f;sums[i]+=f.size;++count[i];}Check(count[0]>0&&count[1]>0&&sums[1]/count[1]>sums[0]/count[0]*1.6f,"DPI sizing mismatch");});
 test("zero density remains empty",[]{auto w=Desktop();Snow s;s.Sync(w);Step(s,w,600);Check(s.Idle(),"zero density spawned snow");});
 test("unevenness setting changes drift terrain",[]{auto w=Desktop();Snow a,b;Cfg().drifts=0;a.Sync(w);Cfg().drifts=100;b.Sync(w);Check(a.surfaces[0].cap!=b.surfaces[0].cap,"terrain setting ignored");});
 test("new transient window receives a light inherited snow cap",[]{auto w=Desktop();Window(w,1,{100,300,900,800});Snow s;s.Sync(w);std::fill(Find(s,1).h.begin(),Find(s,1).h.end(),8);Window(w,2,{950,450,1500,900},true);s.Sync(w);Check(Volume(Find(s,2))>0,"transient cap missing");});
 test("all snow presets match and defaults preserve enabled/language",[]{Cfg().enabled=false;Cfg().lang=4;for(int i=0;i<4;++i){ApplyPreset((Preset)i);Check(CurrentPreset()==i,"preset mismatch");}ApplyPreset((Preset)88);ResetSettings();Check(!Cfg().enabled&&Cfg().lang==4&&Cfg().intensity==45,"defaults lost global choices");});
 std::printf("\n%d passed, %d failed\n",pass,fail);return fail?1:0;
}
