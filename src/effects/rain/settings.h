#pragma once
#include "../../settings_api.h"
#include "../../common.h"

namespace rainfx {

enum RainMode { MODE_ON_TOP = 0, MODE_BEHIND = 1 };

struct Settings {
    bool enabled         = true;
    bool showFalling     = true; // off: waves still build up, falling snow is hidden
    int  mode            = MODE_ON_TOP;
    int  intensity       = 45;   // 0..100
    int  fallSpeed       = 40;   // 0..100
    int  angle           = 10;   // -60..60 degrees
    int  gusts           = 30;   // 0..100
    int  dropSize       = 50;   // 0..100
    int  largeDropShare    = 20;   // 0..100, share of crystal drops
    int  cursorWind      = 50;   // 0..100, wind leans toward the cursor
    int  accumulation    = 50;   // 0..100
    int  waterTaskbar     = 28;   // logical px above the taskbar
    int  waterWindows    = 9;    // logical px on a window edge
    int  evaporation     = 5;    // 0..100
    int  waves           = 45;   // 0..100, animated surface ripples
    bool onWindows       = true;
    bool onTaskbar       = true;
    bool cursorKnock     = true;
    int  cursorRadius    = 26;   // px
    bool pauseFullscreen = true;
    int  fps             = 60;   // 15..144
    int  lang            = -1;   // -1: Windows UI language, otherwise Lang
    bool lightning       = false;
    bool screenFlash     = false;
    int  lightningInterval = 25; // seconds, randomized around this value
    int  lightningBrightness = 55;
    int  lightningBranches = 55;
    int  rainOpacity     = 65;
    int  waterOpacity    = 60;
    int  streakLength    = 50;
    int  splashes        = 60;
};

enum Preset { PRESET_LIGHT, PRESET_NORMAL, PRESET_STRONG, PRESET_BLIZZARD };

Settings& Cfg();
std::vector<SettingField> Fields();


void ResetSettings();
void ApplyPreset(Preset p);
int  CurrentPreset();  // preset matching the current values, or -1




} // namespace rainfx
