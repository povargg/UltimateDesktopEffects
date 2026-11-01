#include "effects.h"
bool Effects::Select(int effect,const World& world){
 if(!ValidEffect(effect)||effect==active_)return false;
 active_=effect;SuiteCfg().effect=effect;SetScale(scale_);Sync(world);return true;
}
void Effects::SetScale(float scale){scale_=scale;switch(active_){case EFFECT_SNOW:snow.SetScale(scale);break;case EFFECT_RAIN:rain.SetScale(scale);break;case EFFECT_LEAVES:leaves.SetScale(scale);break;case EFFECT_HALLOWEEN:halloween.SetScale(scale);break;}}
void Effects::Sync(const World& w){switch(active_){case EFFECT_SNOW:snow.Sync(w);break;case EFFECT_RAIN:rain.Sync(w);break;case EFFECT_LEAVES:leaves.Sync(w);break;case EFFECT_HALLOWEEN:halloween.Sync(w);break;}}
void Effects::Update(float dt,const World& w,POINT cursor,bool valid){
 auto c=ActiveSettings();if(!c.enabled||(c.pauseFullscreen&&w.gameOrFullscreen)||!std::isfinite(dt)||dt<=0)return;
 dt=std::min(dt,.1f);++updates_[active_];
 switch(active_){case EFFECT_SNOW:snow.Update(dt,w,cursor,valid);break;case EFFECT_RAIN:rain.Update(dt,w,cursor,valid);break;case EFFECT_LEAVES:leaves.Update(dt,w,cursor,valid);break;case EFFECT_HALLOWEEN:halloween.Update(dt,w,cursor,valid);break;}
}
void Effects::ClearAll(){switch(active_){case EFFECT_SNOW:snow.ClearAll();break;case EFFECT_RAIN:rain.ClearAll();break;case EFFECT_LEAVES:leaves.ClearAll();break;case EFFECT_HALLOWEEN:halloween.ClearAll();break;}}
void Effects::ShakeAll(){switch(active_){case EFFECT_SNOW:snow.ShakeAll();break;case EFFECT_RAIN:rain.ShakeAll();break;case EFFECT_LEAVES:leaves.ShakeAll();break;case EFFECT_HALLOWEEN:halloween.ShakeAll();break;}}
bool Effects::Idle()const{switch(active_){case EFFECT_SNOW:return snow.Idle();case EFFECT_RAIN:return rain.Idle();case EFFECT_LEAVES:return leaves.Idle();default:return halloween.Idle();}}
bool Effects::VisibleMotion()const{switch(active_){case EFFECT_SNOW:return snow.VisibleMotion();case EFFECT_RAIN:return rain.VisibleMotion();case EFFECT_LEAVES:return leaves.VisibleMotion();default:return halloween.VisibleMotion();}}
bool Effects::CanCutThreads()const{
 const auto& c=halloweenfx::Cfg();return active_==EFFECT_HALLOWEEN&&c.enabled&&c.cutWebs&&c.showFalling&&c.objectOpacity>0&&c.webOpacity>0&&halloweenfx::AllowsFamily(halloweenfx::FAMILY_SPIDER)&&!halloween.webs.empty();
}
bool Effects::CutWebAt(POINT p,const World& w){return active_==EFFECT_HALLOWEEN&&halloween.CutWebAt(p,w);}
bool Effects::SpawnSpider(const World& w){return active_==EFFECT_HALLOWEEN&&!(ActiveSettings().pauseFullscreen&&w.gameOrFullscreen)&&halloween.SpawnWebSpider(w);}
void Effects::FlashNow(const World& w){if(active_==EFFECT_RAIN&&!(ActiveSettings().pauseFullscreen&&w.gameOrFullscreen))rain.FlashNow(w);}
