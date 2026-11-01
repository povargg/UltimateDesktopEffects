#include "render.h"
void Renderer::Shutdown(){snow_.Shutdown();rain_.Shutdown();leaves_.Shutdown();halloween_.Shutdown();active_=-1;}
bool Renderer::Init(HWND hwnd,int w,int h,float scale,int effect){
 Shutdown();if(!ValidEffect(effect))return false;active_=effect;
 bool ok=false;switch(effect){case EFFECT_SNOW:ok=snow_.Init(hwnd,w,h,scale);break;case EFFECT_RAIN:ok=rain_.Init(hwnd,w,h,scale);break;case EFFECT_LEAVES:ok=leaves_.Init(hwnd,w,h,scale);break;case EFFECT_HALLOWEEN:ok=halloween_.Init(hwnd,w,h,scale);break;}
 if(!ok)Shutdown();
 return ok;
}
bool Renderer::Ready()const{switch(active_){case EFFECT_SNOW:return snow_.Ready();case EFFECT_RAIN:return rain_.Ready();case EFFECT_LEAVES:return leaves_.Ready();case EFFECT_HALLOWEEN:return halloween_.Ready();default:return false;}}
bool Renderer::Draw(const Effects& e,const World& w){if(e.Active()!=active_)return false;switch(active_){case EFFECT_SNOW:return snow_.Draw(e.snow,w);case EFFECT_RAIN:return rain_.Draw(e.rain,w);case EFFECT_LEAVES:return leaves_.Draw(e.leaves,w);case EFFECT_HALLOWEEN:return halloween_.Draw(e.halloween,w);default:return false;}}
