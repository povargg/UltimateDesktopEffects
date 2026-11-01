#include "ui.h"
#include "i18n.h"
#include "settings.h"
#include "leafui.h"
#include "../../license.h"
#include "../../effect_selector.h"
#include <shellapi.h>

namespace leavesfx {
enum {BTN_SHAKE=20,BTN_RESET,BTN_DONE,BTN_APPEARANCE,BTN_CLEAR,BTN_LICENSE};
static std::unique_ptr<LeafPanel> g_panel,g_flyout,g_appearance;
static HWND g_notify=nullptr;
static UINT_PTR g_saveTimer=0;
static int g_uiLang=-1;
HWND SettingsWindow(){return g_panel?g_panel->Hwnd():nullptr;}
void RefreshSettingsWindow(){if(g_panel)g_panel->Redraw();if(g_appearance)g_appearance->Redraw();if(g_flyout)g_flyout->Redraw();}
static void CALLBACK SaveTimerProc(HWND,UINT,UINT_PTR id,DWORD){KillTimer(nullptr,id);g_saveTimer=0;SaveSettings();}
static void Changed() {
    if(g_notify)PostMessageW(g_notify,WM_APP_CFGCHANGED,0,0);
    if(g_saveTimer)KillTimer(nullptr,g_saveTimer);
    g_saveTimer=SetTimer(nullptr,0,700,SaveTimerProc);
    RefreshSettingsWindow();
    if(CurrentLang()!=g_uiLang && g_notify)PostMessageW(g_notify,WM_APP_RELOADUI,0,0);
}
static auto Gi(int Settings::*f){return [f]{return Cfg().*f;};}
static auto Si(int Settings::*f){return [f](int v){Cfg().*f=v;};}
static auto Gb(bool Settings::*f){return [f]{return Cfg().*f?1:0;};}
static auto Sb(bool Settings::*f){return [f](int v){Cfg().*f=v!=0;};}
static void MainEvent(int id) {
    if(!id)Changed();
    else if(id==BTN_RESET){ResetSettings();Changed();}
    else if(id==BTN_SHAKE)PostMessageW(g_notify,WM_APP_SHAKE,0,0);
    else if(id==BTN_APPEARANCE)leavesfx::ShowAppearanceWindow(g_notify);
    else if(id==BTN_DONE)PostMessageW(SettingsWindow(),WM_CLOSE,0,0);
}
void ShowSettingsWindow(HWND notify) {
    g_notify=notify;
    if(HWND hwnd=SettingsWindow()){ShowWindow(hwnd,SW_RESTORE);SetForegroundWindow(hwnd);return;}
    g_uiLang=CurrentLang();g_panel=std::make_unique<LeafPanel>();auto& p=*g_panel;
    p.SetHeader(APP_NAME,Tr(S_SUBTITLE));
    ::AddSuiteHeader(p,notify,false);
    const float y0=112,cardH=398;
    p.AddCard({20,y0,436,y0+cardH},Tr(S_CARD_LEAVES));
    p.AddSegment({38,y0+46,418,y0+82},{Tr(S_MODE_TOP),Tr(S_MODE_BEHIND)},Gi(&Settings::mode),Si(&Settings::mode));
    p.AddSegment({38,y0+94,418,y0+126},{Tr(S_TYPE_SAKURA),Tr(S_TYPE_SUMMER),Tr(S_TYPE_AUTUMN)},Gi(&Settings::leafType),Si(&Settings::leafType));
    struct Row {Str label;int Settings::*field;int lo,hi;const wchar_t* unit;};
    const Row falling[]={
        {S_INTENSITY,&Settings::intensity,0,100,L"%"},{S_FALL_SPEED,&Settings::fallSpeed,0,100,L"%"},
        {S_WIND_ANGLE,&Settings::angle,-60,60,L"°"},{S_CURSOR_WIND,&Settings::cursorWind,0,100,L"%"},
        {S_GUSTS,&Settings::gusts,0,100,L"%"},{S_LEAF_SIZE,&Settings::leafSize,0,100,L"%"},{S_VARIETY,&Settings::variety,0,100,L"%"}};
    for(int i=0;i<7;++i){float y=y0+138+i*36;p.AddSlider({38,y,418,y+34},Tr(falling[i].label),falling[i].lo,falling[i].hi,falling[i].unit,Gi(falling[i].field),Si(falling[i].field));}
    p.AddCard({444,y0,860,y0+cardH},Tr(S_CARD_PILES));
    const Row piles[]={
        {S_ACCUMULATION,&Settings::accumulation,0,100,L"%"},{S_PILE_TASKBAR,&Settings::pileTaskbar,0,200,L" px"},
        {S_PILE_WINDOWS,&Settings::pileWindows,0,100,L" px"},{S_ROUGHNESS,&Settings::roughness,0,100,L"%"},
        {S_DECAY,&Settings::decay,0,100,L"%"},{S_CURSOR_RADIUS,&Settings::cursorRadius,8,80,L" px"},
        {S_SMOOTHNESS,&Settings::fps,15,144,L" FPS"}};
    for(int i=0;i<7;++i){float y=y0+48+i*48;p.AddSlider({464,y,840,y+34},Tr(piles[i].label),piles[i].lo,piles[i].hi,piles[i].unit,Gi(piles[i].field),Si(piles[i].field));}
    const float y1=y0+cardH+12;
    p.AddCard({20,y1,860,y1+184},Tr(S_CARD_BEHAVIOR));
    struct Toggle {Str label;bool Settings::*field;};
    const Toggle toggles[]={{S_LEAVES_ON,&Settings::enabled},{S_SHOW_FALLING,&Settings::showFalling},
        {S_ON_WINDOWS,&Settings::onWindows},{S_ON_TASKBAR,&Settings::onTaskbar},{S_CURSOR_KNOCK,&Settings::cursorKnock},{S_PAUSE_GAMES,&Settings::pauseFullscreen}};
    for(int i=0;i<6;++i){float x=i<4?40.0f:460.0f,y=y1+48+(i%4)*32;p.AddToggle({x,y,x+390,y+28},Tr(toggles[i].label),Gb(toggles[i].field),Sb(toggles[i].field));}
    float ya=y1+112,yl=y1+144;
    p.AddToggle({460,ya,850,ya+28},Tr(S_AUTOSTART),[]{return IsAutostartEnabled()?1:0;},[](int v){SetAutostart(v!=0);});
    p.AddText({462,yl+4,580,yl+28},Tr(S_LANGUAGE));
    p.AddSegment({580,yl,846,yl+28},{kLangCodes[0],kLangCodes[1],kLangCodes[2],kLangCodes[3],kLangCodes[4]},[]{return CurrentLang();},[](int v){SetLanguage(v);});
    float fy=y1+198;
    p.AddButton({20,fy,196,fy+38},Tr(S_SHAKE),BTN_SHAKE);
    p.AddButton({208,fy,408,fy+38},Tr(S_APPEARANCE),BTN_APPEARANCE);
    p.AddButton({516,fy,692,fy+38},Tr(S_DEFAULTS),BTN_RESET);
    p.AddButton({704,fy,860,fy+38},Tr(S_DONE),BTN_DONE,true);
    p.onDestroy=[]{SaveSettings();};
    if(!p.Create(Tr(S_SETTINGS_TITLE),880,fy+78,MainEvent))g_panel.reset();
}
void ShowAppearanceWindow(HWND notify) {
    g_notify=notify;
    if(g_appearance && g_appearance->Hwnd()){ShowWindow(g_appearance->Hwnd(),SW_RESTORE);SetForegroundWindow(g_appearance->Hwnd());return;}
    g_uiLang=CurrentLang();g_appearance=std::make_unique<LeafPanel>();auto& p=*g_appearance;
    p.SetHeader(APP_NAME,Tr(S_APPEARANCE));
    ::AddSuiteHeader(p,notify,false);
    p.AddCard({20,112,436,442},Tr(S_CARD_ANIMATION));
    struct Row {Str label;int Settings::*field;};
    const Row rows[]={{S_LEAF_OPACITY,&Settings::leafOpacity},{S_PILE_OPACITY,&Settings::pileOpacity},{S_FLUTTER,&Settings::flutter},{S_TUMBLE,&Settings::tumble}};
    for(int i=0;i<4;++i){float y=160+i*46.0f;p.AddSlider({40,y,416,y+34},Tr(rows[i].label),0,100,L"%",Gi(rows[i].field),Si(rows[i].field));}
    p.AddText({40,364,416,426},Tr(S_LOOK_NOTE));
    p.AddCard({444,112,860,442},Tr(S_CARD_VARIANTS));
    p.AddSegment({464,160,840,194},{Tr(S_TYPE_SAKURA),Tr(S_TYPE_SUMMER),Tr(S_TYPE_AUTUMN)},Gi(&Settings::leafType),Si(&Settings::leafType));
    for(int i=0;i<4;++i)p.AddLeafImage({464+i*94.0f,222,528+i*94.0f,286},-1,i);
    p.AddText({464,312,840,420},Tr(S_VARIANTS_NOTE));
    p.AddButton({20,458,240,496},Tr(S_LICENSE),BTN_LICENSE);
    p.AddButton({444,458,660,496},Tr(S_CLEAR_ALL),BTN_CLEAR);
    p.AddButton({704,458,860,496},Tr(S_DONE),BTN_DONE,true);
    p.onDestroy=[]{SaveSettings();};
    auto event=[](int id){
        if(!id)Changed();
        else if(id==BTN_CLEAR)PostMessageW(g_notify,WM_APP_CLEAR,0,0);
        else if(id==BTN_LICENSE)MessageBoxW(g_appearance->Hwnd(),kLicenseText,L"LayerLeaves / povargg / MIT",MB_OK|MB_ICONINFORMATION);
        else if(id==BTN_DONE)PostMessageW(g_appearance->Hwnd(),WM_CLOSE,0,0);
    };
    if(!p.Create(Tr(S_APPEARANCE),880,536,event))g_appearance.reset();
}
void CloseUi(){if(g_panel && g_panel->Hwnd())DestroyWindow(g_panel->Hwnd());if(g_appearance && g_appearance->Hwnd())DestroyWindow(g_appearance->Hwnd());if(g_flyout && g_flyout->Hwnd())DestroyWindow(g_flyout->Hwnd());
    // Switching happens through a posted host message, after widget callbacks return.
    g_panel.reset();g_flyout.reset();g_appearance.reset();
}
void ReloadUi(){TrayUpdateTip();bool main=SettingsWindow()!=nullptr,appearance=g_appearance && g_appearance->Hwnd();CloseUi();if(main)leavesfx::ShowSettingsWindow(g_notify);if(appearance)leavesfx::ShowAppearanceWindow(g_notify);}
void ShowTrayFlyout(HWND notify) {
    g_notify=notify;
    if(g_flyout && g_flyout->Hwnd()){DestroyWindow(g_flyout->Hwnd());return;}
    g_uiLang=CurrentLang();g_flyout=std::make_unique<LeafPanel>();auto& p=*g_flyout;p.SetHeader(APP_NAME,L"");
    ::AddSuiteHeader(p,notify,true);
    p.AddToggle({18,62,318,90},Tr(S_LEAVES_ON),Gb(&Settings::enabled),Sb(&Settings::enabled));
    p.AddToggle({18,94,318,122},Tr(S_SHOW_FALLING),Gb(&Settings::showFalling),Sb(&Settings::showFalling));
    p.AddToggle({18,126,318,154},Tr(S_AUTOSTART),[]{return IsAutostartEnabled()?1:0;},[](int v){SetAutostart(v!=0);});
    p.AddCaption({20,166,318,184},Tr(S_LEAF_TYPE));
    p.AddSegment({16,188,320,222},{Tr(S_TYPE_SAKURA),Tr(S_TYPE_SUMMER),Tr(S_TYPE_AUTUMN)},Gi(&Settings::leafType),Si(&Settings::leafType));
    p.AddCaption({20,232,318,250},Tr(S_MODE));
    p.AddSegment({16,254,320,288},{Tr(S_MODE_TOP),Tr(S_MODE_BEHIND)},Gi(&Settings::mode),Si(&Settings::mode));
    p.AddCaption({20,298,318,316},Tr(S_SMOOTHNESS_FPS));
    std::vector<std::wstring> fps;for(int f:kFpsChoices)fps.push_back(std::to_wstring(f));
    p.AddSegment({16,320,320,354},fps,[]{for(int i=0;i<kFpsChoiceCount;++i)if(Cfg().fps==kFpsChoices[i])return i;return -1;},[](int v){Cfg().fps=kFpsChoices[v];});
    p.AddItem({8,366,328,396},Tr(S_SHAKE_ALL),CMD_SHAKE);
    p.AddItem({8,398,328,428},Tr(S_APPEARANCE),CMD_APPEARANCE);
    p.AddItem({8,430,328,460},Tr(S_ALL_SETTINGS),CMD_SETTINGS);
    p.AddItem({8,462,328,492},Tr(S_EXIT),CMD_EXIT);
    p.AddLink({20,504,318,524},Tr(S_PROJECT_BY),APP_REPO);
    p.AddLink({20,526,318,546},Tr(S_AUTHOR),UPSTREAM_REPO);
    POINT pt;GetCursorPos(&pt);
    auto event=[](int id){if(!id){Changed();return;}PostMessageW(g_notify,WM_APP_TRAYCMD,id,0);PostMessageW(g_flyout->Hwnd(),WM_CLOSE,0,0);};
    if(!p.Create(APP_NAME,336,570,event,&pt))g_flyout.reset();
}

bool UiContainsPoint(POINT point){for(auto* p:{g_panel.get(),g_flyout.get(),g_appearance.get()}){if(!p||!p->Hwnd()||!IsWindowVisible(p->Hwnd())||IsIconic(p->Hwnd()))continue;RECT r{};if(GetWindowRect(p->Hwnd(),&r)&&PtInRect(&r,point))return true;}return false;}
} // namespace leavesfx
