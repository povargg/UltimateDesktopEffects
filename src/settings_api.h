#pragma once
#include "common.h"
#include <array>

enum Effect { EFFECT_SNOW=0, EFFECT_RAIN=1, EFFECT_LEAVES=2, EFFECT_HALLOWEEN=3, EFFECT_COUNT=4 };
inline bool ValidEffect(int e){return e>=0&&e<EFFECT_COUNT;}
struct SuiteSettings { int effect=EFFECT_SNOW; int lang=-1; };
struct SettingField {const wchar_t* key;int* i;bool* b;int lo,hi;};
SuiteSettings& SuiteCfg();
void SetLanguage(int lang);
int CurrentLang();
void LoadSettings();void SaveSettings();
std::wstring SettingsDir();
bool IsAutostartEnabled();
void SetAutostart(bool on,const std::wstring& exe=ExePath());
std::string EncodeSettings();
void DecodeSettings(const std::string& ini,bool reset=true);
void ResetAllProfiles();
std::vector<SettingField> ProfileFields(int effect);
const char* ProfileName(int effect);
struct CommonSettings {bool enabled,showFalling,pauseFullscreen;int intensity,fps,mode;};
CommonSettings ActiveSettings();
