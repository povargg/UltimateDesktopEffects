#pragma once
#include "../../ui.h"
namespace leavesfx {
void ShowSettingsWindow(HWND);void ShowTrayFlyout(HWND);void ReloadUi();void CloseUi();void RefreshSettingsWindow();HWND SettingsWindow();bool UiContainsPoint(POINT);
void ShowAppearanceWindow(HWND);
}
