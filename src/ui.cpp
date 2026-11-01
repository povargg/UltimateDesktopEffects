#include "ui.h"
#include "i18n.h"
#include "settings.h"
#include "effects/snow/ui.h"
#include "effects/rain/ui.h"
#include "effects/leaves/ui.h"
#include "effects/halloween/ui.h"
#include "license.h"

#include <commctrl.h>
#include <shellapi.h>

HICON AppIcon(bool smallIcon, UINT dpi) {
    HICON icon = nullptr;
    int cx = smallIcon ? GetSystemMetricsForDpi(SM_CXSMICON, dpi) : GetSystemMetricsForDpi(SM_CXICON, dpi);
    LoadIconWithScaleDown(AppInstance(), MAKEINTRESOURCEW(IDI_APP), cx, cx, &icon);
    return icon;
}

// ---------- Tray icon ----------

static NOTIFYICONDATAW g_nid = {};

bool TrayAdd(HWND owner) {
    g_nid = {};
    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd = owner;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
    g_nid.uCallbackMessage = WM_APP_TRAY;
    g_nid.hIcon = AppIcon(true, GetDpiForSystem());
    wcscpy_s(g_nid.szTip, Tr(S_TRAY_TIP));
    if (!Shell_NotifyIconW(NIM_ADD, &g_nid)) return false;
    g_nid.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &g_nid);
    return true;
}

void TrayUpdateTip() {
    if (!g_nid.hWnd) return;
    NOTIFYICONDATAW n = g_nid;
    n.uFlags = NIF_TIP | NIF_SHOWTIP;
    wcscpy_s(n.szTip, Tr(S_TRAY_TIP));
    Shell_NotifyIconW(NIM_MODIFY, &n);
}

void TrayBalloon(const wchar_t* title, const wchar_t* text) {
    if (!g_nid.hWnd) return;
    NOTIFYICONDATAW n = g_nid;
    n.uFlags = NIF_INFO;
    n.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON;
    n.hBalloonIcon = AppIcon(false, GetDpiForSystem());
    wcscpy_s(n.szInfoTitle, title);
    wcscpy_s(n.szInfo, text);
    Shell_NotifyIconW(NIM_MODIFY, &n);
}

void TrayRemove() {
    if (g_nid.hWnd) Shell_NotifyIconW(NIM_DELETE, &g_nid);
    g_nid.hWnd = nullptr;
}

// ---------- Particle settings and appearance ----------
void ShowSettingsWindow(HWND notify){switch(SuiteCfg().effect){case EFFECT_SNOW:snowfx::ShowSettingsWindow(notify);break;case EFFECT_RAIN:rainfx::ShowSettingsWindow(notify);break;case EFFECT_LEAVES:leavesfx::ShowSettingsWindow(notify);break;case EFFECT_HALLOWEEN:halloweenfx::ShowSettingsWindow(notify);break;}}
void ShowAppearanceWindow(HWND notify){switch(SuiteCfg().effect){case EFFECT_SNOW:snowfx::ShowSettingsWindow(notify);break;case EFFECT_RAIN:rainfx::ShowWeatherWindow(notify);break;case EFFECT_LEAVES:leavesfx::ShowAppearanceWindow(notify);break;case EFFECT_HALLOWEEN:halloweenfx::ShowAppearanceWindow(notify);break;}}
void ShowTrayFlyout(HWND notify){switch(SuiteCfg().effect){case EFFECT_SNOW:snowfx::ShowTrayFlyout(notify);break;case EFFECT_RAIN:rainfx::ShowTrayFlyout(notify);break;case EFFECT_LEAVES:leavesfx::ShowTrayFlyout(notify);break;case EFFECT_HALLOWEEN:halloweenfx::ShowTrayFlyout(notify);break;}}
void RefreshSettingsWindow(){switch(SuiteCfg().effect){case EFFECT_SNOW:snowfx::RefreshSettingsWindow();break;case EFFECT_RAIN:rainfx::RefreshSettingsWindow();break;case EFFECT_LEAVES:leavesfx::RefreshSettingsWindow();break;case EFFECT_HALLOWEEN:halloweenfx::RefreshSettingsWindow();break;}}
void ReloadUi(){switch(SuiteCfg().effect){case EFFECT_SNOW:snowfx::ReloadUi();break;case EFFECT_RAIN:rainfx::ReloadUi();break;case EFFECT_LEAVES:leavesfx::ReloadUi();break;case EFFECT_HALLOWEEN:halloweenfx::ReloadUi();break;}}
HWND SettingsWindow(){switch(SuiteCfg().effect){case EFFECT_SNOW:return snowfx::SettingsWindow();case EFFECT_RAIN:return rainfx::SettingsWindow();case EFFECT_LEAVES:return leavesfx::SettingsWindow();default:return halloweenfx::SettingsWindow();}}
void CloseUi(){snowfx::CloseUi();rainfx::CloseUi();leavesfx::CloseUi();halloweenfx::CloseUi();}
bool UiContainsPoint(POINT point){return snowfx::UiContainsPoint(point)||rainfx::UiContainsPoint(point)||leavesfx::UiContainsPoint(point)||halloweenfx::UiContainsPoint(point);}
