#include "leaves.h"
#include "leaf_geometry.h"

namespace leavesfx {
// Surface ownership, occlusion and window following derive from LayerSnow by flipkips (MIT).
static constexpr uintptr_t kGroundKeyBase=~(uintptr_t)0-64;

size_t Leaves::SettledCount() const {size_t n=0;for(const auto& s:surfaces)n+=s.leaves.size();return n;}
float Leaves::ScaleAt(float x,float y) const {
    float distance=1e20f,scale=scale_;
    for(const auto& m:monitors_) {
        float dx=x<m.rc.l?m.rc.l-x:(x>m.rc.r?x-m.rc.r:0);
        float dy=y<m.rc.t?m.rc.t-y:(y>m.rc.b?y-m.rc.b:0);
        if(dx+dy<distance){distance=dx+dy;scale=m.scale;}
    }
    return scale;
}
float Leaves::Support(const Surface& s,float x,float radius) {
    int lo=std::max(0,(int)std::floor((x-radius)/kCell));
    int hi=std::min(s.cells-1,(int)std::floor((x+radius)/kCell));
    float height=0;
    for(int c=lo;c<=hi;++c)height=std::max(height,s.h[c]);
    return height;
}
float Leaves::LayerTop(const SettledLeaf& l,float x) {
    float d=(x-l.x)/std::max(0.1f,l.size*kRestingCollisionHalfWidth);
    if(std::fabs(d)>1)return 0;
    return l.base+l.thickness*(0.45f+0.55f*std::sqrt(std::max(0.0f,1-d*d)));
}
void Leaves::Shape(Surface& s) {
    s.rough=Cfg().roughness/100.0f;
    s.cap.resize(s.cells);
    float phase=(s.key%997)*0.037f;
    for(int c=0;c<s.cells;++c) {
        float x=c*kCell/std::max(0.1f,s.scale);
        float noise=0.5f+0.3f*std::sin(x*0.021f+phase)+0.2f*std::sin(x*0.067f+phase*1.7f);
        s.cap[c]=1.0f-s.rough*0.22f*noise;
    }
}
void Leaves::Rebuild(Surface& s) {
    s.h.assign(s.cells,0);
    size_t kept=0;
    for(auto leaf:s.leaves) {
        if(leaf.x<0 || leaf.x>=s.width || s.maxH<=0)continue;
        int cell=std::clamp((int)(leaf.x/kCell),0,s.cells-1);
        float support=Support(s,leaf.x,leaf.size*kRestingSupportHalfWidth);
        leaf.base=std::max(0.0f,support-leaf.thickness*0.28f);
        if(leaf.base+leaf.thickness>s.maxH*s.cap[cell]+0.001f)continue;
        s.leaves[kept++]=leaf;
        int lo=std::max(0,(int)((leaf.x-leaf.size*kRestingCollisionHalfWidth)/kCell));
        int hi=std::min(s.cells-1,(int)((leaf.x+leaf.size*kRestingCollisionHalfWidth)/kCell));
        for(int c=lo;c<=hi;++c)s.h[c]=std::min(s.maxH,std::max(s.h[c],LayerTop(leaf,(c+0.5f)*kCell)));
    }
    s.leaves.resize(kept);++s.revision;
}
bool Leaves::Land(const Leaf& f,Surface& s,float x) {
    if(Cfg().accumulation<=0 || s.maxH<=0)return true;
    float amount=Cfg().accumulation/50.0f;
    if(amount<1 && Rand01()>amount)return true;
    if(s.leaves.size()>=(s.ground?4096u:1400u) || settledCount_>=kMaxSettled)return false;
    SettledLeaf leaf;
    leaf.x=Clampf(x,0.0f,std::max(0.0f,s.width-0.01f));leaf.size=f.size;leaf.variant=f.variant;
    leaf.rotation=std::round(f.rotation*57.2957795f/22.5f)*22.5f;
    leaf.thickness=f.size*(0.0525f+Cfg().roughness*0.0009f)*RandRange(0.75f,1.2f)*std::max(1.0f,amount*0.85f);
    leaf.thickness=std::min(leaf.thickness,s.maxH*0.6f);
    float support=Support(s,leaf.x,leaf.size*kRestingSupportHalfWidth);
    leaf.base=std::max(0.0f,support-leaf.thickness*0.28f);
    int cell=std::clamp((int)(leaf.x/kCell),0,s.cells-1);
    if(leaf.base+leaf.thickness>s.maxH*s.cap[cell])return false;
    s.leaves.push_back(leaf);++settledCount_;++s.revision;
    int lo=std::max(0,(int)((leaf.x-leaf.size*kRestingCollisionHalfWidth)/kCell));
    int hi=std::min(s.cells-1,(int)((leaf.x+leaf.size*kRestingCollisionHalfWidth)/kCell));
    for(int c=lo;c<=hi;++c)s.h[c]=std::min(s.maxH,std::max(s.h[c],LayerTop(leaf,(c+0.5f)*kCell)));
    return true;
}
void Leaves::Lift(const SettledLeaf& l,const Surface& s,float vx,float vy) {
    if(airborne.size()>=kMaxAirborne)return;
    Leaf f;
    f.x=s.left+l.x;f.y=s.top-RestingLeafCenterHeight(l,Cfg().leafType);f.vx=vx;f.vy=vy;
    f.size=l.size;f.scale=s.scale;f.variant=l.variant;f.kind=LEAF_LIFTED;f.z=1;
    f.phase=RandRange(0,6.2831853f);f.freq=RandRange(0.7f,1.7f);f.spin=RandRange(-3,3);
    f.rotation=l.rotation*0.017453293f;f.life=8;f.ignoreSurface=s.key;
    airborne.push_back(f);
}
void Leaves::Follow(Surface& s,int left,int top,int width,int anchorL,int anchorR) {
    float dt=std::max(0.008f,time_-s.lastSync);
    bool moved=anchorR-anchorL==s.anchorR-s.anchorL;
    float dx=moved?(float)(anchorL-s.anchorL):0.0f,dy=(float)(top-s.top);
    float offset=s.left+dx-left;
    float speed=std::sqrt(dx*dx+dy*dy)/dt;
    float shake=Clampf((speed-900*s.scale)/(6000*s.scale),0,0.45f);
    s.left=left;s.top=top;s.width=width;s.cells=std::max(1,width/kCell);
    size_t kept=0;
    for(auto leaf:s.leaves) {
        leaf.x+=offset;
        if(leaf.x<0 || leaf.x>=width || Rand01()<shake)
            Lift(leaf,s,dx/dt*0.25f+RandRange(-35,35)*s.scale,dy/dt*0.2f-RandRange(30,90)*s.scale);
        else s.leaves[kept++]=leaf;
    }
    s.leaves.resize(kept);Shape(s);Rebuild(s);
}
void Leaves::Occlusion(Surface& s,const World& w,int zLimit) {
    s.hideBelow.assign(s.cells,0);s.visCeil.assign(s.cells,1e9f);
    for(int k=0;k<zLimit && k<(int)w.wins.size();++k) {
        const auto& o=w.wins[k].rc;
        if(o.r<=s.left || o.l>=s.left+s.width)continue;
        int lo=std::max(0,(o.l-s.left)/kCell),hi=std::min(s.cells,(o.r-s.left+kCell-1)/kCell);
        if(o.t<=s.top-1 && o.b>s.top-1) {
            for(int c=lo;c<hi;++c)s.hideBelow[c]=std::max(s.hideBelow[c],(float)(s.top-o.t));
        }else if(o.b<=s.top-1) {
            for(int c=lo;c<hi;++c)s.visCeil[c]=std::min(s.visCeil[c],(float)(s.top-o.b));
        }
    }
}
void Leaves::Sync(const World& w) {
    monitors_=w.monitors;
    std::vector<Surface> next;next.reserve(surfaces.size()+4);
    auto take=[&](uintptr_t key,bool ground,int left,int top,int width,float maxH,int z,int anchorL,int anchorR,float scale) {
        if(width<2)return;
        Surface s;Surface* old=nullptr;
        for(auto& p:surfaces)if(p.key==key){old=&p;break;}
        if(old)s=std::move(*old);
        else {s.key=key;s.ground=ground;s.left=left;s.top=top;s.width=width;s.cells=std::max(1,width/kCell);s.h.assign(s.cells,0);s.anchorL=anchorL;s.anchorR=anchorR;}
        bool rebuild=s.maxH!=maxH || s.scale!=scale || s.rough!=Cfg().roughness/100.0f;
        s.maxH=maxH;s.scale=scale;
        if(old && (s.left!=left || s.top!=top || s.width!=width))Follow(s,left,top,width,anchorL,anchorR);
        else if(rebuild){Shape(s);Rebuild(s);}
        s.anchorL=anchorL;s.anchorR=anchorR;s.lastSync=time_;s.z=z;
        Occlusion(s,w,z);next.push_back(std::move(s));
    };
    if(Cfg().onTaskbar)for(size_t i=0;i<w.monitors.size();++i) {
        const auto& m=w.monitors[i];
        int z=Cfg().mode==MODE_BEHIND?(int)w.wins.size():std::max(0,w.firstTaskbarZ);
        take(kGroundKeyBase+i,true,m.rc.l,m.groundY,m.rc.w(),Cfg().pileTaskbar*m.scale,z,m.rc.l,m.rc.r,m.scale);
    }
    if(Cfg().onWindows)for(size_t z=0;z<w.wins.size();++z) {
        const auto& wi=w.wins[z];if(wi.taskbar)continue;
        const MonitorInfo* mon=nullptr;int cx=(wi.rc.l+wi.rc.r)/2;
        for(const auto& m:w.monitors)if(cx>=m.rc.l && cx<m.rc.r && wi.rc.t>=m.rc.t && wi.rc.t<m.rc.b){mon=&m;break;}
        if(!mon || wi.rc.t<=mon->rc.t+2 || wi.rc.t>=mon->groundY-2)continue;
        int inset=(int)(7*mon->scale),left=std::max(0,wi.rc.l+inset),right=std::min(w.virt.w(),wi.rc.r-inset);
        if(right-left<40)continue;
        take((uintptr_t)wi.hwnd,false,left,wi.rc.t,right-left,std::min(Cfg().pileWindows*mon->scale,(float)(wi.rc.t-mon->rc.t)),(int)z,wi.rc.l,wi.rc.r,mon->scale);
    }
    surfaces=std::move(next);settledCount_=SettledCount();
}
void Leaves::Knock(POINT cur) {
    float scale=ScaleAt((float)cur.x,(float)cur.y),r=Cfg().cursorRadius*scale;
    for(auto& s:surfaces) {
        if(cur.x+r<s.left || cur.x-r>s.left+s.width || cur.y>s.top+r || cur.y<s.top-s.maxH-56*s.scale-r)continue;
        size_t kept=0;
        for(auto l:s.leaves) {
            float x=s.left+l.x,y=s.top-RestingLeafCenterHeight(l,Cfg().leafType);
            float dx=x-cur.x,dy=y-cur.y,reach=r+l.size*0.25f;
            int c=std::clamp((int)(l.x/kCell),0,s.cells-1);
            bool visible=RestingLeafVisibleTop(l,Cfg().leafType)>s.hideBelow[c] && l.base<s.visCeil[c] && cur.y<s.top-s.hideBelow[c]+2;
            if(visible && dx*dx+dy*dy<reach*reach)
                Lift(l,s,curVx_*0.35f+RandRange(-75,75)*scale,curVy_*0.25f-RandRange(35,110)*scale);
            else s.leaves[kept++]=l;
        }
        if(kept!=s.leaves.size()){s.leaves.resize(kept);Rebuild(s);}
    }
    settledCount_=SettledCount();
}
void Leaves::ShakeAll() {
    for(auto& s:surfaces) {
        for(const auto& l:s.leaves)Lift(l,s,RandRange(-100,100)*s.scale,RandRange(-100,-25)*s.scale);
        s.leaves.clear();std::fill(s.h.begin(),s.h.end(),0);++s.revision;
    }
    settledCount_=0;
}
void Leaves::ClearAll() {
    airborne.clear();
    for(auto& s:surfaces){s.leaves.clear();std::fill(s.h.begin(),s.h.end(),0);++s.revision;}
    settledCount_=0;prefilled_=false;spawnAcc_=0;
}
} // namespace leavesfx
