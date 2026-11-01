#pragma once
#include "settings.h"
#include "effects/snow/snow.h"
#include "effects/rain/rain.h"
#include "effects/leaves/leaves.h"
#include "effects/halloween/scene.h"

// One host owns the four CPU states; only the selected state advances.
class Effects {
public:
 snowfx::Snow snow;rainfx::Rain rain;leavesfx::Leaves leaves;halloweenfx::Scene halloween;
 Effects():active_(ValidEffect(SuiteCfg().effect)?SuiteCfg().effect:EFFECT_SNOW){SuiteCfg().effect=active_;}
 int Active()const{return active_;}
 bool Select(int effect,const World& world);
 void Sync(const World& world);
 void Update(float dt,const World& world,POINT cursor,bool cursorValid);
 void SetScale(float scale);
 void ClearAll();void ShakeAll();bool Idle()const;bool VisibleMotion()const;
 bool CutWebAt(POINT point,const World& world);
 bool SpawnSpider(const World& world);void FlashNow(const World& world);
 std::array<float,4> Times()const{return {snow.Time(),rain.Time(),leaves.Time(),halloween.Time()};}
 const std::array<uint64_t,4>& UpdateCounts()const{return updates_;}
 bool CanCutThreads()const;
private:
 int active_;float scale_=1;std::array<uint64_t,4> updates_{};
};
