#include "settings.h"

namespace rainfx {
static Settings config;
Settings& Cfg(){return config;}
std::vector<SettingField> Fields() {
    auto& s=Cfg();
    return {
        {L"enabled", nullptr, &s.enabled, 0, 1},
        {L"showFalling", nullptr, &s.showFalling, 0, 1},
        {L"mode", &s.mode, nullptr, 0, 1},
        {L"intensity", &s.intensity, nullptr, 0, 100},
        {L"fallSpeed", &s.fallSpeed, nullptr, 0, 100},
        {L"angle", &s.angle, nullptr, -60, 60},
        {L"gusts", &s.gusts, nullptr, 0, 100},
        {L"dropSize", &s.dropSize, nullptr, 0, 100},
        {L"largeDropShare", &s.largeDropShare, nullptr, 0, 100},
        {L"cursorWind", &s.cursorWind, nullptr, 0, 100},
        {L"accumulation", &s.accumulation, nullptr, 0, 100},
        {L"waterTaskbar", &s.waterTaskbar, nullptr, 0, 200},
        {L"waterWindows", &s.waterWindows, nullptr, 0, 100},
        {L"evaporation", &s.evaporation, nullptr, 0, 100},
        {L"waves", &s.waves, nullptr, 0, 100},
        {L"onWindows", nullptr, &s.onWindows, 0, 1},
        {L"onTaskbar", nullptr, &s.onTaskbar, 0, 1},
        {L"cursorKnock", nullptr, &s.cursorKnock, 0, 1},
        {L"cursorRadius", &s.cursorRadius, nullptr, 8, 80},
        {L"pauseFullscreen", nullptr, &s.pauseFullscreen, 0, 1},
        {L"fps", &s.fps, nullptr, 15, 144},
        {L"lang", &s.lang, nullptr, -1, 4},
        {L"lightning", nullptr, &s.lightning, 0, 1},
        {L"screenFlash", nullptr, &s.screenFlash, 0, 1},
        {L"lightningInterval", &s.lightningInterval, nullptr, 8, 120},
        {L"lightningBrightness", &s.lightningBrightness, nullptr, 0, 100},
        {L"lightningBranches", &s.lightningBranches, nullptr, 0, 100},
        {L"rainOpacity", &s.rainOpacity, nullptr, 0, 100},
        {L"waterOpacity", &s.waterOpacity, nullptr, 0, 100},
        {L"streakLength", &s.streakLength, nullptr, 0, 100},
        {L"splashes", &s.splashes, nullptr, 0, 100},
    };
}
} // namespace rainfx
