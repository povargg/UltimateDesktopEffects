#pragma once
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#ifdef ULTIMATE_SIMULATION_TEST
#include "../tests/win_stub.h"
#else
#include <windows.h>
#include <wrl/client.h>
#endif
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <string>
#include <utility>
#include <vector>

#ifndef ULTIMATE_SIMULATION_TEST
using Microsoft::WRL::ComPtr;
#endif

#define APP_NAME    L"UltimateDesktopEffects"
#define APP_VERSION L"1.0.1"
#define APP_AUTHOR  L"povargg & flipkips"
#define APP_REPO    L"https://github.com/povargg"
#define UPSTREAM_REPO L"https://github.com/flipkips/desktop-snow-overlay"
#define IDI_APP     101

// Messages for the hidden message window
#define WM_APP_TRAY          (WM_APP + 1)
#define WM_APP_SHOWSETTINGS  (WM_APP + 2)
#define WM_APP_CFGCHANGED    (WM_APP + 3)
#define WM_APP_SHAKE         (WM_APP + 4)
#define WM_APP_TRAYCMD       (WM_APP + 5)  // wParam: TrayCmd from the tray menu
#define WM_APP_RELOADUI      (WM_APP + 6)  // UI language changed
#define WM_APP_MODALLOOP     (WM_APP + 7)  // wParam: 1 when a window drag starts, 0 when it ends
#define WM_APP_CLEAR         (WM_APP + 8)
#define WM_APP_SPIDER        (WM_APP + 9)
#define WM_APP_LIGHTNING     (WM_APP + 10)
#define WM_APP_EFFECT        (WM_APP + 11)

DWORD WindowsBuild();  // 22000+ is Windows 11

constexpr wchar_t kMsgWndClass[] = L"UltimateDesktopEffects.MsgWnd";
constexpr wchar_t kMutexName[]   = L"Local\\UltimateDesktopEffects.SingleInstance";

struct RectI {
    int l = 0, t = 0, r = 0, b = 0;
    int w() const { return r - l; }
    int h() const { return b - t; }
    bool contains(float x, float y) const { return x >= l && x < r && y >= t && y < b; }
};

// Fast PRNG (xorshift32)
inline uint32_t& RngState() { static uint32_t s = 0x9E3779B9u ^ GetTickCount(); return s; }
inline float Rand01() {
    uint32_t& s = RngState();
    s ^= s << 13; s ^= s >> 17; s ^= s << 5;
    return (s >> 8) * (1.0f / 16777216.0f);
}
inline float RandRange(float a, float b) { return a + (b - a) * Rand01(); }

inline float Clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }

std::wstring ExePath();
HINSTANCE AppInstance();
