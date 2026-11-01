#pragma once
#include "../../common.h"
#include "settings.h"
#include "../../world.h"

namespace leavesfx {
constexpr int kCell = 2;
constexpr size_t kMaxAirborne = 6000;
constexpr size_t kMaxSettled = 18000;
// Compact collision footprints allow full-face sprites to overlap in a dense layer.
constexpr float kRestingCollisionHalfWidth = 0.24f;
constexpr float kRestingSupportHalfWidth = 0.18f;
constexpr float kRestingSpriteSquash = 1.0f;
enum LeafKind : uint8_t { LEAF_FALLING, LEAF_LIFTED };
struct Leaf {
    float x=0,y=0,vx=0,vy=0,size=16,z=1,scale=1;
    float phase=0,freq=1,rotation=0,spin=0,age=0,life=30;
    uint8_t variant=0;
    LeafKind kind=LEAF_FALLING;
    uintptr_t ignoreSurface=0;
};
struct SettledLeaf {
    float x=0,size=16,thickness=3,base=0,rotation=0,age=0;
    uint8_t variant=0;
};
struct Surface {
    uintptr_t key=0;
    bool ground=false;
    int left=0,top=0,width=0,cells=0,z=0,anchorL=0,anchorR=0;
    float maxH=0,scale=1,lastSync=0,rough=-1;
    std::vector<float> h,cap,hideBelow,visCeil;
    std::vector<SettledLeaf> leaves;
    uint64_t revision=1;
};
class Leaves {
public:
    std::vector<Leaf> airborne;
    std::vector<Surface> surfaces;
    void Sync(const World& world);
    void Update(float dt,const World& world,POINT cursor,bool cursorValid);
    void ShakeAll();
    void ClearAll();
    bool Idle() const;
    bool VisibleMotion() const;
    void SetScale(float scale) {scale_=scale;}
    float Time() const {return time_;}
    size_t SettledCount() const;
    static float Support(const Surface& s,float x,float radius);
    static float LayerTop(const SettledLeaf& leaf,float x);
private:
    void Spawn(float dt,const World& w);
    void SpawnLeaf(const World& w,bool anywhere);
    void Lift(const SettledLeaf& leaf,const Surface& s,float vx,float vy);
    bool Land(const Leaf& leaf,Surface& s,float localX);
    void Rebuild(Surface& s);
    void Shape(Surface& s);
    void Occlusion(Surface& s,const World& w,int zLimit);
    void Knock(POINT cursor);
    void Follow(Surface& s,int left,int top,int width,int anchorL,int anchorR);
    float ScaleAt(float x,float y) const;
    std::vector<MonitorInfo> monitors_;
    float scale_=1,time_=0,spawnAcc_=0,angle_=0,cursorAngle_=0,curVx_=0,curVy_=0;
    POINT previousCursor_={0,0};
    bool haveCursor_=false,prefilled_=false;
    size_t settledCount_=0;
};
} // namespace leavesfx
