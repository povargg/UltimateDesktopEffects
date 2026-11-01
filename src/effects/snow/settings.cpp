#include "settings.h"

namespace snowfx {
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
        {L"flakeSize", &s.flakeSize, nullptr, 0, 100},
        {L"crystalShare", &s.crystalShare, nullptr, 0, 100},
        {L"cursorWind", &s.cursorWind, nullptr, 0, 100},
        {L"accumulation", &s.accumulation, nullptr, 0, 100},
        {L"pileGround", &s.pileGround, nullptr, 0, 200},
        {L"pileWindows", &s.pileWindows, nullptr, 0, 100},
        {L"melt", &s.melt, nullptr, 0, 100},
        {L"drifts", &s.drifts, nullptr, 0, 100},
        {L"onWindows", nullptr, &s.onWindows, 0, 1},
        {L"onTaskbar", nullptr, &s.onTaskbar, 0, 1},
        {L"cursorKnock", nullptr, &s.cursorKnock, 0, 1},
        {L"cursorRadius", &s.cursorRadius, nullptr, 8, 80},
        {L"pauseFullscreen", nullptr, &s.pauseFullscreen, 0, 1},
        {L"fps", &s.fps, nullptr, 15, 144},
        {L"lang", &s.lang, nullptr, -1, 4},
    };
}
} // namespace snowfx
