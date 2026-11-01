#include "scene.h"
#include "sprite_geometry.h"
#include "../../occlusion.h"

namespace halloweenfx {

void Scene::ApplyContentMode(){
 if(appliedMode_==Cfg().contentMode)return;
 appliedMode_=Cfg().contentMode;prefilled_=false;spawnAcc_=0;nextSpider_=0;
 airborne.erase(std::remove_if(airborne.begin(),airborne.end(),[](const Particle& p){return !AllowsFamily(p.family);}),airborne.end());
 if(!AllowsFamily(FAMILY_SPIDER))webs.clear();
 for(auto& s:surfaces){
  auto end=std::remove_if(s.objects.begin(),s.objects.end(),[](const SettledObject& p){return !AllowsFamily(p.family);});
  if(end!=s.objects.end()){s.objects.erase(end,s.objects.end());Rebuild(s);}
 }
 settledCount_=SettledCount();
}
float Scene::SpiderFloor(float x,float startY,float size,const World& w)const{
 float floor=(float)w.virt.h()-size*.5f-3;
 for(const auto& m:w.monitors)if(x>=m.rc.l&&x<m.rc.r&&startY>=m.rc.t&&startY<m.rc.b){floor=m.groundY-size*.5f-3*m.scale;break;}
 for(const auto& s:surfaces){
  if(x<s.left||x>=s.left+s.width||s.top<=startY+size)continue;
  int c=std::clamp((int)((x-s.left)/kCell),0,s.cells-1);
  if(s.h[c]+.5f<s.hideBelow[c]||s.h[c]>s.visCeil[c])continue;
  floor=std::min(floor,s.top-s.h[c]-size*.5f-3*s.scale);
 }
 return floor;
}
bool Scene::SpawnWebSpider(const World& w){
 if(!Cfg().enabled||!AllowsFamily(FAMILY_SPIDER)||w.monitors.empty()||webs.size()>=kMaxWebSpiders)return false;
 for(int attempt=0;attempt<6;++attempt){
  const auto& m=w.monitors[std::min(w.monitors.size()-1,(size_t)(Rand01()*w.monitors.size()))];
  WebSpider s;s.id=nextSpiderId_++;s.scale=m.scale;s.monitor=m.rc;
  s.size=NativeParticleSize(Cfg().objectSize)*m.scale*RandRange(.95f,1.15f);
  s.anchorX=RandRange(m.rc.l+s.size,m.rc.r-s.size);s.anchorY=(float)m.rc.t;s.x=s.anchorX;s.y=s.anchorY+s.size*.5f+m.scale;
  float floor=SpiderFloor(s.x,s.y,s.size,w);if(floor-s.y<12*m.scale)continue;
  s.variant=(uint8_t)std::min(VariantCount(Cfg().variety,FAMILY_SPIDER)-1,(int)(Rand01()*VariantCount(Cfg().variety,FAMILY_SPIDER)));
  s.partial=Rand01()*100<Cfg().partialDescents;
  s.targetY=s.partial?s.y+(floor-s.y)*RandRange(.2f,.65f):floor;
  s.speed=(8+Cfg().spiderSpeed*.3f)*m.scale;s.wait=Cfg().hangTime*RandRange(.8f,1.2f);s.phase=RandRange(0,6.2831853f);
  webs.push_back(s);return true;
 }
 return false;
}
ThreadSegment Scene::Thread(const WebSpider& s){
 float angle=s.state==SPIDER_ASCENDING?180.0f:0.0f;
 float top=ParticleOpaqueBounds(FAMILY_SPIDER,s.variant,angle).top*s.size;
 return {s.anchorX,s.anchorY,s.x,s.y+top+s.scale};
}
void Scene::ReleaseSpider(const WebSpider& s){
 if(airborne.size()>=kMaxAirborne)return;
 Particle p;p.family=FAMILY_SPIDER;p.variant=s.variant;p.x=s.x;p.y=s.y;p.size=s.size;p.scale=s.scale;p.z=1;
 p.kind=PARTICLE_LIFTED;p.vx=std::cos(s.phase+time_)*4*s.scale;p.vy=std::max(10*s.scale,s.speed*.3f);
 p.rotation=s.state==SPIDER_ASCENDING?3.14159265f:0;p.spin=RandRange(-1,1);p.phase=s.phase;p.freq=1;
 p.life=std::max(12.0f,s.monitor.h()/std::max(1.0f,220*s.scale)+5);airborne.push_back(p);
}
void Scene::UpdateSpiders(float dt,const World& w){
 if(!AllowsFamily(FAMILY_SPIDER)){webs.clear();return;}
 if(Cfg().intensity>0){
  nextSpider_-=dt;
  if(nextSpider_<=0){
   SpawnWebSpider(w);
   nextSpider_=Cfg().spiderInterval*(45.0f/std::max(10,Cfg().intensity))*RandRange(.7f,1.3f)*(Cfg().contentMode==CONTENT_SPIDERS?.7f:1.0f);
  }
 }
 for(size_t i=0;i<webs.size();){
  auto& s=webs[i];s.age+=dt;s.speed=(8+Cfg().spiderSpeed*.3f)*s.scale;
  s.x=s.anchorX+std::sin(time_*.7f+s.phase)*Cfg().gusts*.025f*s.scale;
  bool gone=s.age>30+s.monitor.h()/std::max(1.0f,s.speed)*2.6f;
  bool stillOnMonitor=false;
  for(const auto& m:w.monitors)if(m.rc.contains(s.anchorX,s.anchorY)){stillOnMonitor=true;break;}
  gone|=!stillOnMonitor;
  if(!gone&&s.state==SPIDER_DESCENDING){
   float floor=SpiderFloor(s.x,s.y,s.size,w);
   if(floor>s.y+2*s.scale)s.targetY=std::min(s.targetY,floor);
   s.y=std::min(s.targetY,s.y+s.speed*dt);
   if(s.y>=s.targetY-.01f)s.state=SPIDER_HANGING;
  }else if(!gone&&s.state==SPIDER_HANGING){
   s.wait-=dt;
   if(s.wait<=0){if(ChooseClimb())s.state=SPIDER_ASCENDING;else{ReleaseSpider(s);gone=true;}}
  }else if(!gone&&s.state==SPIDER_ASCENDING){
   s.y-=s.speed*1.2f*dt;
   float bottom=ParticleOpaqueBounds(FAMILY_SPIDER,s.variant,180).bottom*s.size;
   if(s.y+bottom<s.anchorY-1)gone=true;
  }
  if(gone){webs.erase(webs.begin()+i);}else ++i;
 }
}
static float DistanceSquared(float px,float py,float ax,float ay,float bx,float by){
 float dx=bx-ax,dy=by-ay,len=dx*dx+dy*dy;
 float t=len>.0001f?Clampf(((px-ax)*dx+(py-ay)*dy)/len,0,1):0;
 float x=px-(ax+dx*t),y=py-(ay+dy*t);return x*x+y*y;
}
bool Scene::CutWebAt(POINT point,const World& w){
 if(!Cfg().enabled||!Cfg().cutWebs||!Cfg().showFalling||Cfg().webOpacity<=0||Cfg().objectOpacity<=0||!AllowsFamily(FAMILY_SPIDER)||(Cfg().pauseFullscreen&&w.gameOrFullscreen))return false;
 if(Cfg().mode==MODE_BEHIND)for(const auto& win:w.wins)if(win.rc.contains((float)point.x,(float)point.y))return false;
 size_t nearest=webs.size();float best=1e30f;
 for(size_t i=0;i<webs.size();++i){
  const auto& spider=webs[i];if(!spider.monitor.contains((float)point.x,(float)point.y))continue;
  auto t=Thread(spider);if(t.by<=t.ay+2*spider.scale)continue;
  auto spans=Cfg().mode==MODE_BEHIND?VisibleLine(t.ax,t.ay,t.bx,t.by,0,w):std::vector<std::pair<float,float>>{{0,1}};
  for(auto span:spans){
   float ax=t.ax+(t.bx-t.ax)*span.first,ay=t.ay+(t.by-t.ay)*span.first;
   float bx=t.ax+(t.bx-t.ax)*span.second,by=t.ay+(t.by-t.ay)*span.second;
   float d=DistanceSquared((float)point.x,(float)point.y,ax,ay,bx,by),r=Cfg().cutRadius*spider.scale;
   if(d<=r*r&&d<best){best=d;nearest=i;}
  }
 }
 if(nearest==webs.size())return false;
 ReleaseSpider(webs[nearest]);webs.erase(webs.begin()+nearest);return true;
}
} // namespace halloweenfx
