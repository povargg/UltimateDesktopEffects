#pragma once
#include "../../common.h"
#include "settings.h"
#include "../../world.h"

namespace halloweenfx {
constexpr int kCell=2;
constexpr size_t kMaxAirborne=6000,kMaxSettled=18000,kMaxWebSpiders=24;
constexpr float kRestingCollisionHalfWidth=.24f,kRestingSupportHalfWidth=.18f,kRestingSpriteSquash=1;
enum ParticleKind:uint8_t {PARTICLE_FALLING,PARTICLE_LIFTED};
struct Particle {
 float x=0,y=0,vx=0,vy=0,size=16,z=1,scale=1,phase=0,freq=1,rotation=0,spin=0,age=0,life=30;
 uint8_t variant=0,family=FAMILY_CANDY;ParticleKind kind=PARTICLE_FALLING;uintptr_t ignoreSurface=0;
};
struct SettledObject {
 float x=0,size=16,thickness=3,base=0,rotation=0,age=0;
 uint8_t variant=0,family=FAMILY_CANDY;
};
struct Surface {
 uintptr_t key=0;bool ground=false;
 int left=0,top=0,width=0,cells=0,z=0,anchorL=0,anchorR=0;
 float maxH=0,scale=1,lastSync=0,rough=-1;
 std::vector<float> h,cap,hideBelow,visCeil;std::vector<SettledObject> objects;uint64_t revision=1;
};
enum SpiderState:uint8_t {SPIDER_DESCENDING,SPIDER_HANGING,SPIDER_ASCENDING};
struct WebSpider {
 uint64_t id=0;float anchorX=0,anchorY=0,x=0,y=0,targetY=0,size=16,scale=1,speed=20,wait=3,phase=0,age=0;
 uint8_t variant=0;SpiderState state=SPIDER_DESCENDING;bool partial=false;RectI monitor;
};
struct ThreadSegment {float ax,ay,bx,by;};
class Scene {
public:
 Scene(){airborne.reserve(kMaxAirborne);webs.reserve(kMaxWebSpiders);}
 std::vector<Particle> airborne;std::vector<Surface> surfaces;std::vector<WebSpider> webs;
 void Sync(const World&);void Update(float dt,const World&,POINT cursor,bool cursorValid);
 void ShakeAll();void ClearAll();bool Idle()const;bool VisibleMotion()const;
 void SetScale(float s){scale_=s;}float Time()const{return time_;}size_t SettledCount()const;
 bool SpawnWebSpider(const World&);bool CutWebAt(POINT,const World&);
 static ThreadSegment Thread(const WebSpider&);static bool ChooseClimb(){return Rand01()<.5f;}
 static float Support(const Surface&,float x,float radius);static float LayerTop(const SettledObject&,float x);
private:
 void Spawn(float dt,const World&);void SpawnParticle(const World&,bool anywhere);
 void Lift(const SettledObject&,const Surface&,float vx,float vy);bool Land(const Particle&,Surface&,float localX);
 void Rebuild(Surface&);void Shape(Surface&);void Occlusion(Surface&,const World&,int zLimit);
 void Knock(POINT);void Follow(Surface&,int left,int top,int width,int anchorL,int anchorR);float ScaleAt(float x,float y)const;
 void ApplyContentMode();void UpdateSpiders(float dt,const World&);void ReleaseSpider(const WebSpider&);
 float SpiderFloor(float x,float startY,float size,const World&)const;
 std::vector<MonitorInfo> monitors_;float scale_=1,time_=0,spawnAcc_=0,angle_=0,cursorAngle_=0,curVx_=0,curVy_=0;
 POINT previousCursor_={0,0};bool haveCursor_=false,prefilled_=false;
 size_t settledCount_=0;int appliedMode_=-1;float nextSpider_=0;uint64_t nextSpiderId_=1;
};
} // namespace halloweenfx
