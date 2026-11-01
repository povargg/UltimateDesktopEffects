#include "settings.h"

namespace halloweenfx {
static Settings config;
Settings& Cfg(){return config;}
std::vector<SettingField> Fields() {
    auto& s=Cfg();
    return {
        {L"enabled", nullptr, &s.enabled, 0, 1},
        {L"showFalling", nullptr, &s.showFalling, 0, 1},
        {L"mode", &s.mode, nullptr, 0, 1},
        {L"contentMode", &s.contentMode, nullptr, 0, 2},
        {L"intensity", &s.intensity, nullptr, 0, 100},
        {L"fallSpeed", &s.fallSpeed, nullptr, 0, 100},
        {L"angle", &s.angle, nullptr, -60, 60},
        {L"gusts", &s.gusts, nullptr, 0, 100},
        {L"objectSize", &s.objectSize, nullptr, 0, 100},
        {L"variety", &s.variety, nullptr, 0, 100},
        {L"cursorWind", &s.cursorWind, nullptr, 0, 100},
        {L"accumulation", &s.accumulation, nullptr, 0, 100},
        {L"pileTaskbar", &s.pileTaskbar, nullptr, 0, 200},
        {L"pileWindows", &s.pileWindows, nullptr, 0, 100},
        {L"decay", &s.decay, nullptr, 0, 100},
        {L"roughness", &s.roughness, nullptr, 0, 100},
        {L"onWindows", nullptr, &s.onWindows, 0, 1},
        {L"onTaskbar", nullptr, &s.onTaskbar, 0, 1},
        {L"cursorKnock", nullptr, &s.cursorKnock, 0, 1},
        {L"cursorRadius", &s.cursorRadius, nullptr, 8, 80},
        {L"pauseFullscreen", nullptr, &s.pauseFullscreen, 0, 1},
        {L"fps", &s.fps, nullptr, 15, 144},
        {L"lang", &s.lang, nullptr, -1, 4},
        {L"objectOpacity", &s.objectOpacity, nullptr, 0, 100},
        {L"pileOpacity", &s.pileOpacity, nullptr, 0, 100},
        {L"flutter", &s.flutter, nullptr, 0, 100},
        {L"tumble", &s.tumble, nullptr, 0, 100},
        {L"spiderInterval", &s.spiderInterval, nullptr, 4, 90},
        {L"spiderSpeed", &s.spiderSpeed, nullptr, 0, 100},
        {L"partialDescents", &s.partialDescents, nullptr, 0, 100},
        {L"hangTime", &s.hangTime, nullptr, 1, 12},
        {L"webOpacity", &s.webOpacity, nullptr, 0, 100},
        {L"cutWebs", nullptr, &s.cutWebs, 0, 1},
        {L"cutRadius", &s.cutRadius, nullptr, 2, 12},
    };
}
} // namespace halloweenfx
