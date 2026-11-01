#include "../src/effects/leaves/leaves.h"
#include "../src/occlusion.h"
#include "../src/effects/leaves/leaf_geometry.h"
#include <cstdio>
#include <functional>
#include <numeric>
#include <stdexcept>
using namespace leavesfx;
static Settings config;
Settings& leavesfx::Cfg(){return config;}
static void Check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
static void Reset(){config=Settings();config.intensity=0;config.angle=0;config.gusts=0;config.cursorWind=0;config.flutter=0;config.tumble=0;RngState()=1234567;}
static World Desktop(int width=1920,int height=1080,float scale=1){World w;w.virt={0,0,width,height};MonitorInfo m;m.rc=w.virt;m.work={0,0,width,height-(int)(40*scale)};m.groundY=m.work.b;m.scale=scale;w.monitors.push_back(m);return w;}
static void Window(World& w,uintptr_t key,RectI rect){w.wins.push_back({(HWND)key,rect,false,false});}
static Surface& Find(Leaves& leaves,uintptr_t key){for(auto& s:leaves.surfaces)if(s.key==key)return s;throw std::runtime_error("missing surface");}
static void Step(Leaves& leaves,World& w,int frames,float dt=1.0f/60){for(int i=0;i<frames;++i)leaves.Update(dt,w,{0,0},false);}
static Leaf Flying(float x,float y,float speed=100){Leaf f;f.x=x;f.y=y;f.vy=speed;f.z=1;f.size=16;f.life=60;f.phase=0.5f;f.freq=1;f.spin=1;return f;}
static void Deposit(Leaves& leaves,World& w,uintptr_t key,int count,float localX=100){
 for(int i=0;i<count;++i){auto& s=Find(leaves,key);int c=std::clamp((int)(localX/kCell),0,s.cells-1);
  auto f=Flying(s.left+localX,s.top-s.h[c]-16*0.18f-5,900);f.variant=i%4;f.scale=s.scale;
  leaves.airborne.push_back(f);Step(leaves,w,1,1.0f/30);
 }
}
static void Finite(const Leaves& r){
 Check(r.airborne.size()<=kMaxAirborne,"airborne budget exceeded");Check(r.SettledCount()<=kMaxSettled,"pile budget exceeded");
 for(auto& s:r.surfaces){Check(s.h.size()==(size_t)s.cells,"invalid height grid");for(float h:s.h)Check(std::isfinite(h)&&h>=0&&h<=s.maxH+0.001f,"invalid pile height");
  for(auto l:s.leaves)Check(l.variant<4&&std::isfinite(l.base)&&l.base>=0&&l.base+l.thickness<=s.maxH+0.001f,"invalid settled leaf");}
 for(auto f:r.airborne)Check(f.variant<4&&std::isfinite(f.x)&&std::isfinite(f.y)&&std::isfinite(f.rotation),"invalid flight state");
}
int main(){
 int passed=0,failed=0;
 auto test=[&](const char* name,std::function<void()> fn){Reset();try{fn();++passed;std::printf("PASS %s\n",name);}catch(const std::exception& e){++failed;std::printf("FAIL %s: %s\n",name,e.what());}};
 test("native leaf size maps to 8,16,32 pixels",[]{Check(NativeLeafSize(0)==8&&NativeLeafSize(50)==16&&NativeLeafSize(100)==32,"size mapping wrong");Check(NativeLeafSize(-9)==8&&NativeLeafSize(900)==32,"size clamp wrong");});
 test("variant diversity stays within four sprites",[]{Check(VariantCount(0)==1&&VariantCount(100)==4&&VariantCount(-1)==1&&VariantCount(101)==4,"variant count wrong");});
 test("reset retains language and master switch",[]{Cfg().lang=4;Cfg().enabled=false;Cfg().leafType=0;ResetSettings();Check(Cfg().lang==4&&!Cfg().enabled&&Cfg().leafType==LEAF_AUTUMN,"reset wrong");});
 test("leaf moves downward at zero wind",[]{auto w=Desktop();Leaves r;r.airborne.push_back(Flying(500,100));Step(r,w,60);Check(r.airborne.size()==1&&r.airborne[0].y>150,"leaf did not fall");});
 test("flutter produces horizontal swaying",[]{Cfg().flutter=100;auto w=Desktop();Leaves r;r.airborne.push_back(Flying(500,100));Step(r,w,120);Check(std::fabs(r.airborne[0].x-500)>2,"flutter missing");});
 test("zero flutter and wind keep a vertical trajectory",[]{auto w=Desktop();Leaves r;r.airborne.push_back(Flying(500,100));Step(r,w,120);Check(std::fabs(r.airborne[0].x-500)<0.001f,"unexpected lateral motion");});
 test("tumbling can be disabled",[]{auto w=Desktop();Leaves r;r.airborne.push_back(Flying(500,100));Step(r,w,120);Check(r.airborne[0].rotation==0,"rotation was not disabled");});
 test("tumbling rotates flying leaves",[]{Cfg().tumble=100;auto w=Desktop();Leaves r;r.airborne.push_back(Flying(500,100));Step(r,w,120);Check(r.airborne[0].rotation>1.9f,"rotation missing");});
 test("sakura petals fall more gently than summer leaves",[]{auto w=Desktop();Cfg().leafType=LEAF_SAKURA;Leaves a;a.airborne.push_back(Flying(500,100));Step(a,w,120);Cfg().leafType=LEAF_SUMMER;Leaves b;b.airborne.push_back(Flying(500,100));Step(b,w,120);Check(a.airborne[0].y<b.airborne[0].y-15,"species fall speeds identical");});
 test("spawning uses all four generated variants",[]{Cfg().intensity=60;auto w=Desktop();Leaves r;r.Sync(w);Step(r,w,1);bool seen[4]={};for(auto f:r.airborne)seen[f.variant]=true;for(bool x:seen)Check(x,"variant not used");});
 test("zero density leaves an empty scene idle",[]{auto w=Desktop();Leaves r;r.Sync(w);Step(r,w,300);Check(r.Idle()&&r.airborne.empty()&&r.SettledCount()==0,"zero density not honored");});
 test("leaves collect on windows and the taskbar",[]{Cfg().intensity=100;auto w=Desktop();Window(w,1,{250,350,950,850});Leaves r;r.Sync(w);Step(r,w,3600);Check(Find(r,1).leaves.size()>5,"window stayed empty");Check(r.surfaces[0].ground&&!r.surfaces[0].leaves.empty(),"taskbar stayed empty");Finite(r);});
 test("new leaves rest on previously deposited leaves",[]{auto w=Desktop();Window(w,1,{200,300,900,800});Leaves r;r.Sync(w);Deposit(r,w,1,5);auto& s=Find(r,1);Check(s.leaves.size()>=3,"too few settled leaves");Check(s.leaves.back().base>s.leaves.front().base+3,"stacking did not grow");Finite(r);});
 test("stacked leaves form an uneven layer",[]{auto w=Desktop();Window(w,1,{200,300,900,800});Leaves r;r.Sync(w);Deposit(r,w,1,3,120);Deposit(r,w,1,2,160);Deposit(r,w,1,7,200);auto& s=Find(r,1);float low=1e9f,high=0;for(float h:s.h)if(h>0){low=std::min(low,h);high=std::max(high,h);}Check(high-low>0.5f,"layer is uniformly flat");});
 test("full layers shed excess leaves without exceeding height",[]{Cfg().pileWindows=8;auto w=Desktop();Window(w,1,{200,300,900,800});Leaves r;r.Sync(w);Deposit(r,w,1,80);bool shed=false;for(auto f:r.airborne)shed|=f.kind==LEAF_LIFTED;Check(shed,"full layer did not shed excess");Finite(r);});
 test("slow window movement carries the whole pile",[]{auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;r.Sync(w);Deposit(r,w,1,4,120);Step(r,w,60);size_t count=Find(r,1).leaves.size();w.wins[0].rc={200,340,1000,790};r.Sync(w);Check(Find(r,1).leaves.size()==count&&Find(r,1).left==207&&Find(r,1).top==340,"pile did not follow window");});
 test("fast window shake ejects individual leaves",[]{auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;r.Sync(w);Deposit(r,w,1,30,120);size_t before=Find(r,1).leaves.size();r.Sync(w);w.wins[0].rc={600,300,1400,750};r.Sync(w);Check(Find(r,1).leaves.size()<before&&!r.airborne.empty(),"window shaking did not eject leaves");});
 test("resizing ejects leaves beyond the new edge",[]{auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;r.Sync(w);Deposit(r,w,1,3,650);w.wins[0].rc={100,300,500,750};r.Sync(w);Check(Find(r,1).leaves.empty()&&!r.airborne.empty(),"resize kept clipped leaves");});
 test("minimized or closed windows lose their pile",[]{auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;r.Sync(w);Deposit(r,w,1,3);w.wins.clear();r.Sync(w);Check(r.surfaces.size()==1&&r.surfaces[0].ground,"closed window retained a pile");});
 test("surface switches exclude windows and taskbar",[]{auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;Cfg().onWindows=false;Cfg().onTaskbar=false;r.Sync(w);Check(r.surfaces.empty(),"disabled surfaces remain");});
 test("zero accumulation creates no leaf layer",[]{Cfg().accumulation=0;auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;r.Sync(w);Deposit(r,w,1,20);Check(r.SettledCount()==0,"zero accumulation ignored");});
 test("hidden falling leaves still build piles",[]{auto w=Desktop();Window(w,1,{100,300,900,750});Leaves a;a.Sync(w);Deposit(a,w,1,4);auto count=a.SettledCount();Reset();Cfg().showFalling=false;Leaves b;b.Sync(w);Deposit(b,w,1,4);Check(b.SettledCount()==count&&count>0,"pile-only mode changed deposition");});
 test("cursor brushes a pile into airborne leaves",[]{auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;r.Sync(w);Deposit(r,w,1,5,120);auto before=r.SettledCount();r.Update(1.0f/60,w,{227,295},true);Check(r.SettledCount()<before&&!r.airborne.empty(),"cursor did not brush leaves");Finite(r);});
 test("leaf decay removes old layers",[]{Cfg().decay=100;auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;r.Sync(w);Deposit(r,w,1,4);Step(r,w,1200);Check(r.SettledCount()==0,"decay did not remove old leaves");Finite(r);});
 test("15 FPS collision selects the first of two edges",[]{auto w=Desktop();Window(w,1,{100,300,900,500});Window(w,2,{100,500,900,850});Leaves r;r.Sync(w);auto f=Flying(400,245,1800);r.airborne.push_back(f);Step(r,w,1,1.0f/15);Check(Find(r,1).leaves.size()==1&&Find(r,2).leaves.empty(),"collision skipped or chose wrong edge");});
 test("mixed DPI and changing frames remain finite",[]{Cfg().intensity=100;Cfg().fallSpeed=100;Cfg().angle=60;Cfg().gusts=100;Cfg().flutter=100;Cfg().tumble=100;auto w=Desktop(5760,2160);w.monitors[0].rc={0,0,1920,1080};w.monitors[0].groundY=1040;MonitorInfo m;m.rc={1920,0,5760,2160};m.work={1920,0,5760,2080};m.groundY=2080;m.scale=2;w.monitors.push_back(m);Window(w,1,{200,300,1700,900});Window(w,2,{2200,600,5000,1800});Leaves r;r.Sync(w);for(int i=0;i<2400;++i){r.Update(i%2?1.0f/15:1.0f/144,w,{i%5700,750},true);if(i%45==0)r.Sync(w);if(i%100==0)Finite(r);}Finite(r);});
 test("clear removes particles and every pile",[]{auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;r.Sync(w);Deposit(r,w,1,4);r.ClearAll();Check(r.Idle()&&r.airborne.empty()&&r.SettledCount()==0,"clear left leaves");for(auto& s:r.surfaces)for(float h:s.h)Check(h==0,"clear left collision height");});
 test("shake all preserves leaf sprite identity",[]{auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;r.Sync(w);Deposit(r,w,1,4);auto count=r.SettledCount();r.ShakeAll();Check(r.SettledCount()==0&&r.airborne.size()>=count,"shake lost leaves");bool seen[4]={};for(auto f:r.airborne)seen[f.variant]=true;for(bool x:seen)Check(x,"shake lost a variant");});
 test("background regions exclude the foreground window",[]{auto w=Desktop(100,100);Window(w,1,{25,25,75,75});auto rects=VisibleDesktopRects(w.virt,w);int area=0;for(auto r:rects)area+=r.w()*r.h();Check(rects.size()==4&&area==7500,"background clipping leaks onto a window");});
 test("zero and long frame intervals stay bounded",[]{auto w=Desktop();Leaves r;r.airborne.push_back(Flying(400,100));r.Update(0,w,{0,0},false);Check(r.airborne[0].y==100,"zero time moved leaves");r.Update(10,w,{0,0},false);Check(r.airborne[0].y<120,"long frame was not clamped");Finite(r);});
 test("covered lower edges do not catch foreground leaves",[]{auto w=Desktop();Window(w,1,{200,100,800,800});Window(w,2,{100,400,1000,900});Leaves r;r.Sync(w);auto f=Flying(400,390,800);r.airborne.push_back(f);Step(r,w,1,1.0f/15);Check(Find(r,2).leaves.empty(),"covered edge captured a foreground leaf");});
 test("changing leaf type keeps the existing stack",[]{auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;r.Sync(w);Deposit(r,w,1,4);auto count=r.SettledCount();auto height=Find(r,1).h;for(int type=0;type<3;++type){Cfg().leafType=type;r.Sync(w);Check(r.SettledCount()==count&&Find(r,1).h==height,"type switch erased or moved the pile");}});
 test("settled sprites keep their full opaque face above the edge",[]{
  Check(kRestingSpriteSquash==1.0f,"resting sprites are vertically flattened");
  for(int type=0;type<3;++type)for(int variant=0;variant<4;++variant)for(int frame=0;frame<16;++frame){
   SettledLeaf leaf;leaf.size=32;leaf.base=7;leaf.variant=variant;leaf.rotation=frame*22.5f;
   const auto& b=LeafOpaqueBounds(type,variant,leaf.rotation);
   float center=RestingLeafCenterHeight(leaf,type);
   Check(std::fabs(center-b.bottom*leaf.size-leaf.base)<0.001f,"opaque pixels penetrate the window edge");
   Check(RestingLeafVisibleTop(leaf,type)>leaf.base+leaf.size*0.3f,"visible leaf face has invalid bounds");
  }
 });
 test("compact pile hitboxes overlap full sprites and retain the landing angle",[]{
  Check(kRestingCollisionHalfWidth<0.3f&&kRestingSupportHalfWidth<0.25f,"collision footprint was not reduced");
  auto w=Desktop();Window(w,1,{100,300,900,750});Leaves r;r.Sync(w);
  Deposit(r,w,1,5);const auto& stack=Find(r,1).leaves;
  Check(stack.size()==5,"compact stack lost leaves");
  Check(stack.back().base<8,"compact stack leaves excessive vertical gaps");
  for(const auto& leaf:stack)Check(leaf.rotation==0,"landing changed a leaf's rotation");
  Finite(r);
 });
 std::printf("\n%d passed, %d failed\n",passed,failed);return failed?1:0;
}
