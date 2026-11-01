#pragma once
#include "../../settings_api.h"
#include "../../common.h"

namespace halloweenfx {
enum OverlayMode {MODE_ON_TOP=0,MODE_BEHIND=1};
enum ContentMode {CONTENT_CANDY=0,CONTENT_SPIDERS=1,CONTENT_BOTH=2};
enum SpriteFamily {FAMILY_CANDY=0,FAMILY_SPIDER=1};
struct Settings {
 bool enabled=true,showFalling=true;
 int mode=MODE_ON_TOP,contentMode=CONTENT_BOTH;
 int intensity=45,fallSpeed=40,angle=10,gusts=30,objectSize=50,variety=100,cursorWind=50;
 int accumulation=50,pileTaskbar=48,pileWindows=22,decay=0,roughness=60;
 bool onWindows=true,onTaskbar=true,cursorKnock=true;
 int cursorRadius=26;
 bool pauseFullscreen=true;
 int fps=60,lang=-1,objectOpacity=100,pileOpacity=100,flutter=35,tumble=65;
 int spiderInterval=12,spiderSpeed=45,partialDescents=45,hangTime=3,webOpacity=80;
 bool cutWebs=true;
 int cutRadius=4;
};
Settings& Cfg();
std::vector<SettingField> Fields();
void ResetSettings();

inline float NativeParticleSize(int value){value=std::clamp(value,0,100);return value<=50?8+value*.16f:16+(value-50)*.32f;}
inline int VariantCount(int variety,int family=FAMILY_CANDY){return 1+std::clamp(variety,0,100)*(family==FAMILY_SPIDER?3:5)/100;}
inline bool AllowsFamily(int family){return Cfg().contentMode==CONTENT_BOTH || (Cfg().contentMode==CONTENT_CANDY && family==FAMILY_CANDY) || (Cfg().contentMode==CONTENT_SPIDERS && family==FAMILY_SPIDER);}
} // namespace halloweenfx
