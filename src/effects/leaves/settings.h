#pragma once
#include "../../settings_api.h"
#include "../../common.h"

namespace leavesfx {
enum LeafMode { MODE_ON_TOP = 0, MODE_BEHIND = 1 };
enum LeafType { LEAF_SAKURA = 0, LEAF_SUMMER = 1, LEAF_AUTUMN = 2 };
struct Settings {
    bool enabled = true;
    bool showFalling = true;
    int mode = MODE_ON_TOP;
    int leafType = LEAF_AUTUMN;
    int intensity = 45;
    int fallSpeed = 40;
    int angle = 10;
    int gusts = 30;
    int leafSize = 50;          // 0 = 8px, 50 = 16px, 100 = 32px (before DPI/depth)
    int variety = 100;          // 0..100: one to four variants
    int cursorWind = 50;
    int accumulation = 50;
    int pileTaskbar = 48;
    int pileWindows = 22;
    int decay = 0;              // zero preserves leaves indefinitely
    int roughness = 60;
    bool onWindows = true;
    bool onTaskbar = true;
    bool cursorKnock = true;
    int cursorRadius = 26;
    bool pauseFullscreen = true;
    int fps = 60;
    int lang = -1;
    int leafOpacity = 100;
    int pileOpacity = 100;
    int flutter = 65;
    int tumble = 65;
};
Settings& Cfg();
std::vector<SettingField> Fields();


void ResetSettings();



inline float NativeLeafSize(int value) {
    value = std::clamp(value, 0, 100);
    return value <= 50 ? 8.0f + value * 0.16f : 16.0f + (value - 50) * 0.32f;
}
inline int VariantCount(int variety) { return 1 + std::clamp(variety, 0, 100) * 3 / 100; }
} // namespace leavesfx
