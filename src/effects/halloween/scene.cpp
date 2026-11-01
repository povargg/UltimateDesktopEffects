#include "scene.h"
#include "sprite_geometry.h"

namespace halloweenfx {
static constexpr float kPi=3.141592653589793f;
static float FallSpeed(float scale) {
    return (28.0f+Cfg().fallSpeed*2.2f)*scale;
}
void Scene::SpawnParticle(const World& w,bool anywhere) {
    if(airborne.size()>=kMaxAirborne || w.monitors.empty())return;
    float total=0;for(const auto& m:w.monitors)total+=m.LogicalArea();
    float pick=Rand01()*total;const MonitorInfo* selected=&w.monitors.back();
    for(const auto& m:w.monitors){pick-=m.LogicalArea();if(pick<=0){selected=&m;break;}}
    const auto& m=*selected;
    Particle f;f.scale=m.scale;f.z=RandRange(0.45f,1);
    f.size=NativeParticleSize(Cfg().objectSize)*m.scale*(0.72f+0.34f*f.z)*RandRange(0.86f,1.12f);
    f.variant=(uint8_t)std::min(VariantCount(Cfg().variety)-1,(int)(Rand01()*VariantCount(Cfg().variety)));
    float drift=m.rc.h()*std::tan(angle_*kPi/180);
    f.x=RandRange(std::min((float)m.rc.l,m.rc.l-drift)-25,std::max((float)m.rc.r,m.rc.r-drift)+25);
    f.y=anywhere?RandRange(m.rc.t-20.0f,m.rc.t+m.rc.h()*0.92f):m.rc.t-f.size-10;
    f.vy=FallSpeed(m.scale)*(0.65f+0.35f*f.z);f.vx=f.vy*std::tan(angle_*kPi/180);
    f.phase=RandRange(0,2*kPi);f.freq=RandRange(0.7f,1.6f);f.rotation=RandRange(-kPi,kPi);f.spin=RandRange(-2.2f,2.2f);
    f.life=std::max(8.0f,(m.rc.h()+80)/std::max(1.0f,f.vy)*1.7f+10);
    airborne.push_back(f);
}
void Scene::Spawn(float dt,const World& w) {
    if(!AllowsFamily(FAMILY_CANDY)){prefilled_=false;spawnAcc_=0;return;}
    float target=0,rate=0;
    for(const auto& m:w.monitors) {
        float n=Cfg().intensity*5.0f*m.LogicalArea()/(1920.0f*1080.0f);
        target+=n;rate+=n*FallSpeed(m.scale)*0.8f/std::max(40,m.rc.h());
    }
    if(target>kMaxAirborne*0.7f){rate*=kMaxAirborne*0.7f/target;target=kMaxAirborne*0.7f;}
    if(target<1){prefilled_=false;spawnAcc_=0;return;}
    if(!prefilled_){for(int i=0;i<(int)(target*0.4f);++i)SpawnParticle(w,true);prefilled_=true;}
    size_t live=0;for(const auto& f:airborne)live+=f.kind==PARTICLE_FALLING;
    spawnAcc_=std::min(100.0f,spawnAcc_+rate*dt);
    while(spawnAcc_>=1 && live<target*1.2f && airborne.size()<kMaxAirborne){SpawnParticle(w,false);spawnAcc_-=1;++live;}
}
void Scene::Update(float dt,const World& w,POINT cur,bool cursorValid) {
    dt=Clampf(dt,0,0.1f);if(dt<=0)return;
    ApplyContentMode();time_+=dt;settledCount_=SettledCount();
    if(cursorValid && haveCursor_) {
        float k=std::min(1.0f,dt*12);
        curVx_+=((cur.x-previousCursor_.x)/dt-curVx_)*k;curVy_+=((cur.y-previousCursor_.y)/dt-curVy_)*k;
    }else curVx_=curVy_=0;
    previousCursor_=cur;haveCursor_=cursorValid;
    float cursorTarget=0;
    if(cursorValid && Cfg().cursorWind>0)for(const auto& m:w.monitors) {
        if(!m.rc.contains((float)cur.x,(float)cur.y))continue;
        float nx=(cur.x-(m.rc.l+m.rc.r)*0.5f)/std::max(1.0f,m.rc.w()*0.5f);
        cursorTarget=Clampf(nx,-1,1)*Cfg().cursorWind*0.4f;break;
    }
    cursorAngle_+=(cursorTarget-cursorAngle_)*std::min(1.0f,dt*1.2f);
    angle_=Clampf(Cfg().angle+cursorAngle_,-75,75);
    Spawn(dt,w);if(cursorValid && Cfg().cursorKnock)Knock(cur);
    float tanA=std::tan(angle_*kPi/180);
    float gust=Cfg().gusts*0.008f*FallSpeed(1)*(std::sin(time_*0.37f)*0.65f+std::sin(time_*0.91f+1.7f)*0.35f);
    float pushR=Cfg().cursorRadius*ScaleAt((float)cur.x,(float)cur.y)*2.2f;
    for(size_t i=0;i<airborne.size();) {
        Particle& f=airborne[i];f.age+=dt;float px=f.x,py=f.y;
        if(f.kind==PARTICLE_LIFTED) {
            f.vy=std::min(f.vy+210*f.scale*dt,300*f.scale);
            f.vx*=std::max(0.0f,1-dt*0.45f);
        }else {
            float speed=FallSpeed(f.scale)*(0.65f+0.35f*f.z);
            float flutter=Cfg().flutter/100.0f;
            float targetX=speed*tanA+gust*f.scale+std::sin(time_*f.freq*2+f.phase)*flutter*48*f.scale;
            float targetY=speed*(1+std::sin(time_*f.freq+f.phase)*flutter*0.22f);
            float k=std::min(1.0f,dt*3.5f);
            f.vx+=(targetX-f.vx)*k;f.vy+=(targetY-f.vy)*k;
            if(cursorValid && Cfg().cursorWind>0) {
                float dx=f.x-cur.x,dy=f.y-cur.y,d2=dx*dx+dy*dy;
                if(d2<pushR*pushR) {
                    float effect=(1-std::sqrt(d2)/pushR)*Cfg().cursorWind/100.0f;
                    f.vx+=curVx_*effect*dt*0.6f;f.vy+=curVy_*effect*dt*0.2f;
                }
            }
        }
        f.rotation+=f.spin*dt*Cfg().tumble/100.0f;
        if(std::fabs(f.rotation)>1000)f.rotation=std::fmod(f.rotation,2*kPi);
        f.x+=f.vx*dt;f.y+=f.vy*dt;
        bool gone=f.age>=f.life || f.y>w.virt.h()+80 || f.x<-2500 || f.x>w.virt.w()+2500;
        Surface* hit=nullptr;float firstT=2,hitX=0;
        float contactOffset=ParticleOpaqueBounds(f.family,f.variant,f.rotation*57.2957795f).bottom*f.size;
        if(!gone && f.vy>0)for(auto& s:surfaces) {
            if(f.ignoreSurface==s.key || s.cells<1 || (!s.ground && f.z<0.55f))continue;
            float t=Clampf((s.top-py-contactOffset)/std::max(0.0001f,f.y-py),0,1);
            float x=px+(f.x-px)*t;
            if(x<s.left || x>=s.left+s.width)continue;
            int cell=std::clamp((int)((x-s.left)/kCell),0,s.cells-1);
            float edge=s.top-s.h[cell]-contactOffset;
            if(py>edge || f.y<edge)continue;
            t=(edge-py)/std::max(0.0001f,f.y-py);x=px+(f.x-px)*t;
            if(x<s.left || x>=s.left+s.width)continue;
            cell=std::clamp((int)((x-s.left)/kCell),0,s.cells-1);
            if(s.h[cell]+0.5f<s.hideBelow[cell] || s.h[cell]>s.visCeil[cell])continue;
            if(t<firstT){firstT=t;hit=&s;hitX=x;}
        }
        if(hit) {
            Particle landed=f;landed.x=hitX;
            if(Land(landed,*hit,hitX-hit->left))gone=true;
            else {
                // Full piles shed surplus leaves. Ignore this edge on the way to lower surfaces.
                f.kind=PARTICLE_LIFTED;f.ignoreSurface=hit->key;f.x=hitX;
                int cell=std::clamp((int)((hitX-hit->left)/kCell),0,hit->cells-1);
                f.y=hit->top-hit->h[cell]-contactOffset;
                float dir=hitX<hit->left+hit->width*0.5f?-1.0f:1.0f;
                f.vx=dir*RandRange(55,130)*f.scale;f.vy=-RandRange(15,45)*f.scale;f.age=0;f.life=8;
            }
        }
        if(gone){airborne[i]=airborne.back();airborne.pop_back();}else ++i;
    }
    if(Cfg().decay>0) {
        float life=15+std::pow((100-Cfg().decay)/100.0f,2)*240;
        for(auto& s:surfaces) {
            for(auto& l:s.objects)l.age+=dt;
            auto end=std::remove_if(s.objects.begin(),s.objects.end(),[&](const SettledObject& l){return l.age>=life;});
            if(end!=s.objects.end()){s.objects.erase(end,s.objects.end());Rebuild(s);}
        }
    }
    UpdateSpiders(dt,w);
    settledCount_=SettledCount();
}
bool Scene::Idle() const {return airborne.empty() && webs.empty() && SettledCount()==0;}
bool Scene::VisibleMotion() const {
    if(Cfg().showFalling && !webs.empty())return true;
    for(const auto& f:airborne)if(Cfg().showFalling || f.kind==PARTICLE_LIFTED)return true;
    return false;
}
} // namespace halloweenfx
