#pragma once
#include "../../ui.h"
namespace snowfx {
void ShowSettingsWindow(HWND);void ShowTrayFlyout(HWND);void ReloadUi();void CloseUi();void RefreshSettingsWindow();HWND SettingsWindow();bool UiContainsPoint(POINT);
}
