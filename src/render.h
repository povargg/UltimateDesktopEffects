#pragma once
#include "effects.h"
#include "effects/snow/render.h"
#include "effects/rain/render.h"
#include "effects/leaves/render.h"
#include "effects/halloween/render.h"
// Inactive renderers contain no GPU objects. A switch releases the old device first.
class Renderer {
public:
 bool Init(HWND hwnd,int width,int height,float scale,int effect);
 bool Ready()const;
 void Shutdown();
 bool Draw(const Effects& effects,const World& world);
private:
 int active_=-1;
 snowfx::Renderer snow_;rainfx::Renderer rain_;leavesfx::Renderer leaves_;halloweenfx::Renderer halloween_;
};
