#include "settings.h"

namespace snowfx {
void ResetSettings() {
    bool enabled = Cfg().enabled;
    int lang = Cfg().lang;
    Cfg() = Settings();
    Cfg().enabled = enabled;
    Cfg().lang = lang;
}

struct PresetValues { int intensity, fallSpeed, gusts, angle, accumulation; };
static const PresetValues kPresets[4] = {
    {18, 28, 15, 5, 30},    // light
    {45, 40, 30, 10, 50},   // normal
    {75, 55, 45, 18, 70},   // heavy
    {100, 80, 85, 38, 90},  // blizzard
};

void ApplyPreset(Preset p) {
    if(p<PRESET_LIGHT || p>PRESET_BLIZZARD)return;
    const PresetValues& v = kPresets[p];
    Cfg().intensity = v.intensity;
    Cfg().fallSpeed = v.fallSpeed;
    Cfg().gusts = v.gusts;
    Cfg().angle = v.angle;
    Cfg().accumulation = v.accumulation;
}

int CurrentPreset() {
    for (int i = 0; i < 4; ++i) {
        const PresetValues& v = kPresets[i];
        if (Cfg().intensity == v.intensity && Cfg().fallSpeed == v.fallSpeed && Cfg().gusts == v.gusts &&
            Cfg().angle == v.angle && Cfg().accumulation == v.accumulation)
            return i;
    }
    return -1;
}

} // namespace snowfx
