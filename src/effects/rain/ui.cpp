#include "ui.h"
#include "i18n.h"
#include "settings.h"
#include "weatherui.h"
#include "../../license.h"
#include "../../effect_selector.h"
#include <shellapi.h>

namespace rainfx {
enum { BTN_PRESET0 = 10, BTN_SHAKE = 20, BTN_RESET, BTN_DONE, BTN_WEATHER, BTN_LIGHTNING, BTN_LICENSE };

static std::unique_ptr<WeatherPanel> g_panel, g_flyout, g_weather;
static HWND g_notify = nullptr;
static UINT_PTR g_saveTimer = 0;
static int g_uiLang = -1;  // language the open windows were built with

HWND SettingsWindow() { return g_panel ? g_panel->Hwnd() : nullptr; }
void RefreshSettingsWindow() { if (g_panel) g_panel->Redraw(); if (g_weather) g_weather->Redraw(); }

static void CALLBACK SaveTimerProc(HWND, UINT, UINT_PTR id, DWORD) {
    KillTimer(nullptr, id);
    g_saveTimer = 0;
    SaveSettings();
}

static void Changed() {
    if (g_notify) PostMessageW(g_notify, WM_APP_CFGCHANGED, 0, 0);
    if (g_saveTimer) KillTimer(nullptr, g_saveTimer);
    g_saveTimer = SetTimer(nullptr, 0, 700, SaveTimerProc);  // save once the slider is released
    if (CurrentLang() != g_uiLang && g_notify) PostMessageW(g_notify, WM_APP_RELOADUI, 0, 0);
}

static void OnPanelEvent(int id) {
    if (id == 0) {
        Changed();
    } else if (id >= BTN_PRESET0 && id < BTN_PRESET0 + 4) {
        ApplyPreset((Preset)(id - BTN_PRESET0));
        Changed();
    } else if (id == BTN_RESET) {
        ResetSettings();
        Changed();
    } else if (id == BTN_SHAKE) {
        if (g_notify) PostMessageW(g_notify, WM_APP_SHAKE, 0, 0);
    } else if (id == BTN_WEATHER) {
        rainfx::ShowWeatherWindow(g_notify);
    } else if (id == BTN_DONE) {
        PostMessageW(SettingsWindow(), WM_CLOSE, 0, 0);
    }
    RefreshSettingsWindow();
}

void ShowSettingsWindow(HWND notify) {
    g_notify = notify;
    if (HWND w = SettingsWindow()) {
        ShowWindow(w, SW_RESTORE);
        SetForegroundWindow(w);
        return;
    }
    using F = std::function<int()>;
    using S = std::function<void(int)>;
    auto gi = [](int Settings::*f) -> F { return [f] { return Cfg().*f; }; };
    auto si = [](int Settings::*f) -> S { return [f](int v) { Cfg().*f = v; }; };
    auto gb = [](bool Settings::*f) -> F { return [f] { return Cfg().*f ? 1 : 0; }; };
    auto sb = [](bool Settings::*f) -> S { return [f](int v) { Cfg().*f = v != 0; }; };

    g_uiLang = CurrentLang();
    g_panel = std::make_unique<WeatherPanel>();
    WeatherPanel& p = *g_panel;
    p.SetHeader(APP_NAME, Tr(S_SUBTITLE));
    ::AddSuiteHeader(p,notify,false);

    const float y0 = 112, cardH = 398;
    const D2D1_RECT_F L = {20, y0, 436, y0 + cardH};
    p.AddCard(L, Tr(S_CARD_SNOWFALL));
    p.AddSegment({L.left + 18, y0 + 46, L.right - 18, y0 + 82}, {Tr(S_MODE_TOP), Tr(S_MODE_BEHIND)},
                 gi(&Settings::mode), si(&Settings::mode));
    const float pw = (L.right - L.left - 36 - 3 * 8) / 4;
    for (int i = 0; i < 4; ++i) {
        float x = L.left + 18 + i * (pw + 8);
        p.AddButton({x, y0 + 94, x + pw, y0 + 126}, Tr((Str)(S_PRESET_LIGHT + i)), BTN_PRESET0 + i);
    }
    struct Row { Str label; int Settings::*f; int lo, hi; const wchar_t* unit; };
    const Row fall[] = {
        {S_INTENSITY, &Settings::intensity, 0, 100, L"%"},
        {S_FALL_SPEED, &Settings::fallSpeed, 0, 100, L"%"},
        {S_WIND_ANGLE, &Settings::angle, -60, 60, L"°"},
        {S_CURSOR_WIND, &Settings::cursorWind, 0, 100, L"%"},
        {S_GUSTS, &Settings::gusts, 0, 100, L"%"},
        {S_FLAKE_SIZE, &Settings::dropSize, 0, 100, L"%"},
        {S_CRYSTALS, &Settings::largeDropShare, 0, 100, L"%"},
    };
    for (int i = 0; i < 7; ++i) {
        float y = y0 + 138 + i * 36;
        p.AddSlider({L.left + 18, y, L.right - 18, y + 34}, Tr(fall[i].label), fall[i].lo, fall[i].hi, fall[i].unit,
                    gi(fall[i].f), si(fall[i].f));
    }

    const D2D1_RECT_F R = {444, y0, 860, y0 + cardH};
    p.AddCard(R, Tr(S_CARD_PILES));
    const Row piles[] = {
        {S_ACCUMULATION, &Settings::accumulation, 0, 100, L"%"},
        {S_PILE_TASKBAR, &Settings::waterTaskbar, 0, 200, L" px"},
        {S_PILE_WINDOWS, &Settings::waterWindows, 0, 100, L" px"},
        {S_DRIFTS, &Settings::waves, 0, 100, L"%"},
        {S_MELT, &Settings::evaporation, 0, 100, L"%"},
        {S_CURSOR_RADIUS, &Settings::cursorRadius, 8, 80, L" px"},
        {S_SMOOTHNESS, &Settings::fps, 15, 144, L" FPS"},
    };
    for (int i = 0; i < 7; ++i) {
        float y = y0 + 48 + i * 48;
        p.AddSlider({R.left + 18, y, R.right - 18, y + 34}, Tr(piles[i].label), piles[i].lo, piles[i].hi,
                    piles[i].unit, gi(piles[i].f), si(piles[i].f));
    }

    const float y1 = y0 + cardH + 12;
    p.AddCard({20, y1, 860, y1 + 184}, Tr(S_CARD_BEHAVIOR));
    struct Tog { Str label; bool Settings::*f; };
    const Tog toggles[] = {
        {S_SNOW_ON, &Settings::enabled},           {S_SHOW_FALLING, &Settings::showFalling},
        {S_ON_WINDOWS, &Settings::onWindows},      {S_ON_TASKBAR, &Settings::onTaskbar},
        {S_CURSOR_KNOCK, &Settings::cursorKnock},  {S_PAUSE_GAMES, &Settings::pauseFullscreen},
    };
    for (int i = 0; i < 6; ++i) {
        float x = i < 4 ? 40.0f : 460.0f, y = y1 + 48 + (i % 4) * 32;
        p.AddToggle({x, y, x + 390, y + 28}, Tr(toggles[i].label), gb(toggles[i].f), sb(toggles[i].f));
    }
    const float ya = y1 + 48 + 2 * 32, yl = y1 + 48 + 3 * 32;
    p.AddToggle({460, ya, 850, ya + 28}, Tr(S_AUTOSTART), [] { return IsAutostartEnabled() ? 1 : 0; },
                [](int v) { SetAutostart(v != 0); });
    p.AddText({462, yl + 4, 580, yl + 28}, Tr(S_LANGUAGE));
    p.AddSegment({580, yl, 846, yl + 28}, {kLangCodes[0], kLangCodes[1], kLangCodes[2], kLangCodes[3], kLangCodes[4]},
                 [] { return CurrentLang(); }, [](int v) { SetLanguage(v); });

    const float fy = y1 + 184 + 14;
    p.AddButton({20, fy, 196, fy + 38}, Tr(S_SHAKE), BTN_SHAKE);
    p.AddButton({208, fy, 408, fy + 38}, Tr(S_WEATHER_SETTINGS), BTN_WEATHER);
    p.AddButton({516, fy, 692, fy + 38}, Tr(S_DEFAULTS), BTN_RESET);
    p.AddButton({704, fy, 860, fy + 38}, Tr(S_DONE), BTN_DONE, true);

    p.onDestroy = [] { SaveSettings(); };  // the object itself is freed when the window is opened next time
    if (!p.Create(Tr(S_SETTINGS_TITLE), 880, fy + 38 + 40, OnPanelEvent)) g_panel.reset();
}

void ShowWeatherWindow(HWND notify) {
    g_notify = notify;
    if (g_weather && g_weather->Hwnd()) {
        ShowWindow(g_weather->Hwnd(), SW_RESTORE); SetForegroundWindow(g_weather->Hwnd()); return;
    }
    g_uiLang = CurrentLang();
    g_weather = std::make_unique<WeatherPanel>();
    auto& p = *g_weather;
    auto gi = [](int Settings::*f) { return [f] { return Cfg().*f; }; };
    auto si = [](int Settings::*f) { return [f](int v) { Cfg().*f = v; }; };
    auto gb = [](bool Settings::*f) { return [f] { return Cfg().*f ? 1 : 0; }; };
    auto sb = [](bool Settings::*f) { return [f](int v) { Cfg().*f = v != 0; }; };
    p.SetHeader(APP_NAME, Tr(S_WEATHER_SETTINGS));
    ::AddSuiteHeader(p,notify,false);
    p.AddCard({20, 112, 436, 442}, Tr(S_CARD_LIGHTNING));
    p.AddToggle({40, 160, 416, 190}, Tr(S_LIGHTNING), gb(&Settings::lightning), sb(&Settings::lightning));
    p.AddToggle({40, 194, 416, 224}, Tr(S_SCREEN_FLASH), gb(&Settings::screenFlash), sb(&Settings::screenFlash));
    p.AddSlider({40, 236, 416, 270}, Tr(S_LIGHTNING_INTERVAL), 8, 120, L" s", gi(&Settings::lightningInterval), si(&Settings::lightningInterval));
    p.AddSlider({40, 276, 416, 310}, Tr(S_LIGHTNING_BRIGHTNESS), 0, 100, L"%", gi(&Settings::lightningBrightness), si(&Settings::lightningBrightness));
    p.AddSlider({40, 316, 416, 350}, Tr(S_LIGHTNING_BRANCHES), 0, 100, L"%", gi(&Settings::lightningBranches), si(&Settings::lightningBranches));
    p.AddText({40, 364, 416, 426}, Tr(S_LIGHTNING_NOTE));
    p.AddCard({444, 112, 860, 442}, Tr(S_CARD_LOOK));
    struct Row { Str label; int Settings::*f; };
    const Row rows[] = {{S_RAIN_OPACITY, &Settings::rainOpacity}, {S_WATER_OPACITY, &Settings::waterOpacity},
                        {S_STREAK_LENGTH, &Settings::streakLength}, {S_SPLASHES, &Settings::splashes}};
    for (int i = 0; i < 4; ++i) {
        float y = 160.0f + i * 46;
        p.AddSlider({464, y, 840, y + 34}, Tr(rows[i].label), 0, 100, L"%", gi(rows[i].f), si(rows[i].f));
    }
    p.AddText({464, 364, 840, 426}, Tr(S_LOOK_NOTE));
    p.AddButton({20, 458, 220, 496}, Tr(S_LICENSE), BTN_LICENSE);
    p.AddButton({444, 458, 640, 496}, Tr(S_TEST_LIGHTNING), BTN_LIGHTNING);
    p.AddButton({704, 458, 860, 496}, Tr(S_DONE), BTN_DONE, true);
    p.onDestroy = [] { SaveSettings(); };
    auto event = [](int id) {
        if (!id) { Changed(); RefreshSettingsWindow(); if (g_flyout) g_flyout->Redraw(); }
        else if (id == BTN_LIGHTNING) PostMessageW(g_notify, WM_APP_LIGHTNING, 0, 0);
        else if (id == BTN_LICENSE) MessageBoxW(g_weather->Hwnd(), kLicenseText, L"LayerRain / MIT License", MB_OK | MB_ICONINFORMATION);
        else if (id == BTN_DONE) PostMessageW(g_weather->Hwnd(), WM_CLOSE, 0, 0);
    };
    if (!p.Create(Tr(S_WEATHER_SETTINGS), 880, 536, event)) g_weather.reset();
}

void CloseUi() {
    if (g_panel && g_panel->Hwnd()) DestroyWindow(g_panel->Hwnd());
    if (g_weather && g_weather->Hwnd()) DestroyWindow(g_weather->Hwnd());
    if (g_flyout && g_flyout->Hwnd()) DestroyWindow(g_flyout->Hwnd());

    // Switching happens through a posted host message, after widget callbacks return.
    g_panel.reset();g_flyout.reset();g_weather.reset();
}

void ReloadUi() {
    TrayUpdateTip();
    bool settings = SettingsWindow() != nullptr;
    bool weather = g_weather && g_weather->Hwnd();
    CloseUi();
    if (settings) rainfx::ShowSettingsWindow(g_notify);
    if (weather) rainfx::ShowWeatherWindow(g_notify);
}

// ---------- Tray menu (right click) ----------

void ShowTrayFlyout(HWND notify) {
    g_notify = notify;
    if (HWND w = g_flyout ? g_flyout->Hwnd() : nullptr) {  // a second click closes it
        DestroyWindow(w);
        return;
    }
    g_uiLang = CurrentLang();
    g_flyout = std::make_unique<WeatherPanel>();
    WeatherPanel& p = *g_flyout;
    p.SetHeader(APP_NAME, L"");
    ::AddSuiteHeader(p,notify,true);

    p.AddToggle({18, 62, 298, 90}, Tr(S_SNOW_ON),
                [] { return Cfg().enabled ? 1 : 0; }, [](int v) { Cfg().enabled = v != 0; });
    p.AddToggle({18, 92, 298, 120}, Tr(S_SHOW_FALLING),
                [] { return Cfg().showFalling ? 1 : 0; }, [](int v) { Cfg().showFalling = v != 0; });
    p.AddToggle({18, 122, 298, 150}, Tr(S_AUTOSTART),
                [] { return IsAutostartEnabled() ? 1 : 0; }, [](int v) { SetAutostart(v != 0); });

    p.AddToggle({18, 152, 298, 180}, Tr(S_LIGHTNING),
        [] { return Cfg().lightning ? 1 : 0; }, [](int v) { Cfg().lightning = v != 0; });

    p.AddCaption({20, 192, 298, 210}, Tr(S_MODE));
    p.AddSegment({16, 212, 300, 244}, {Tr(S_MODE_TOP), Tr(S_MODE_BEHIND)},
                 [] { return Cfg().mode; }, [](int v) { Cfg().mode = v; });
    p.AddCaption({20, 252, 298, 270}, Tr(S_SNOWFALL_STRENGTH));
    p.AddSegment({16, 272, 300, 304},
                 {Tr(S_PRESET_LIGHT), Tr(S_PRESET_NORMAL), Tr(S_PRESET_STRONG), Tr(S_PRESET_BLIZZARD)},
                 [] { return CurrentPreset(); }, [](int v) { ApplyPreset((Preset)v); });
    p.AddCaption({20, 312, 298, 330}, Tr(S_SMOOTHNESS_FPS));
    std::vector<std::wstring> fps;
    for (int f : kFpsChoices) fps.push_back(std::to_wstring(f));
    p.AddSegment({16, 332, 300, 364}, fps,
                 [] {
                     for (int i = 0; i < kFpsChoiceCount; ++i)
                         if (Cfg().fps == kFpsChoices[i]) return i;
                     return -1;
                 },
                 [](int v) { Cfg().fps = kFpsChoices[v]; });

    p.AddItem({8, 376, 308, 406}, Tr(S_SHAKE_ALL), CMD_SHAKE);
    p.AddItem({8, 408, 308, 438}, Tr(S_WEATHER_SETTINGS), CMD_WEATHER);
    p.AddItem({8, 440, 308, 470}, Tr(S_ALL_SETTINGS), CMD_SETTINGS);
    p.AddItem({8, 472, 308, 502}, Tr(S_EXIT), CMD_EXIT);
    p.AddLink({20,514,150,534},L"povargg",APP_REPO);
    p.AddLink({160,514,298,534},L"flipkips",UPSTREAM_REPO,true);

    auto onEvent = [](int id) {
        if (id == 0) {
            Changed();
            RefreshSettingsWindow();
            if (g_flyout) g_flyout->Redraw();
            return;
        }
        PostMessageW(g_notify, WM_APP_TRAYCMD, id, 0);
        PostMessageW(g_flyout->Hwnd(), WM_CLOSE, 0, 0);
    };
    POINT pt;
    GetCursorPos(&pt);
    if (!p.Create(APP_NAME, 316, 562, onEvent, &pt)) g_flyout.reset();
}

bool UiContainsPoint(POINT point){for(auto* p:{g_panel.get(),g_flyout.get(),g_weather.get()}){if(!p||!p->Hwnd()||!IsWindowVisible(p->Hwnd())||IsIconic(p->Hwnd()))continue;RECT r{};if(GetWindowRect(p->Hwnd(),&r)&&PtInRect(&r,point))return true;}return false;}
} // namespace rainfx
