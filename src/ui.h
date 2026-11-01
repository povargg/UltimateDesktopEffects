#pragma once
#include "common.h"
enum TrayCmd {CMD_SHAKE=1001,CMD_SETTINGS,CMD_EXIT,CMD_APPEARANCE,CMD_CLEAR,CMD_SPIDER};
constexpr int CMD_WEATHER=CMD_APPEARANCE;
constexpr int kFpsChoices[]={20,30,45,60,90,120,144};
constexpr int kFpsChoiceCount=sizeof(kFpsChoices)/sizeof(kFpsChoices[0]);
HICON AppIcon(bool smallIcon,UINT dpi=96);
bool TrayAdd(HWND);void TrayRemove();void TrayBalloon(const wchar_t*,const wchar_t*);void TrayUpdateTip();
void ShowSettingsWindow(HWND);void ShowAppearanceWindow(HWND);void ShowTrayFlyout(HWND);void CloseUi();void ReloadUi();void RefreshSettingsWindow();HWND SettingsWindow();bool UiContainsPoint(POINT);
