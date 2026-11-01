#include "../src/effects/rain/rain.h"
#include "../src/occlusion.h"
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <stdexcept>
#include <numeric>

using namespace rainfx;
static Settings config;
Settings& rainfx::Cfg() { return config; }
static void Check(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
static void Reset() {
    config = Settings(); config.cursorWind = 0; config.gusts = 0; config.evaporation = 0;
    RngState() = 1234567;
}
static World Desktop(int width = 1920, int height = 1080, float scale = 1) {
    World w; w.virt = {0, 0, width, height};
    MonitorInfo m; m.rc = w.virt; m.work = {0, 0, width, height - (int)(40 * scale)};
    m.groundY = m.work.b; m.scale = scale; w.monitors.push_back(m); return w;
}
static void Window(World& w, uintptr_t id, RectI rect) { w.wins.push_back({(HWND)id, rect, false, false}); }
static float Volume(const Surface& s) { return std::accumulate(s.h.begin(), s.h.end(), 0.0f) * kCell; }
static float Volume(const Rain& r) { float total = 0; for (auto& s : r.surfaces) total += Volume(s); return total; }
static Surface& Find(Rain& r, uintptr_t key) {
    for (auto& s : r.surfaces) if (s.key == key) return s;
    throw std::runtime_error("surface missing");
}
static void Step(Rain& r, World& w, int frames, float dt = 1.0f/60) {
    for (int i = 0; i < frames; ++i) r.Update(dt, w, {0, 0}, false);
}
static void Finite(const Rain& r) {
    Check(r.drops.size() <= 7000, "particle budget exceeded");
    Check(r.splashes.size() <= 1200, "splash budget exceeded");
    for (const auto& s : r.surfaces) for (float h : s.h)
        Check(std::isfinite(h) && h >= -0.0001f && h <= s.maxH + 0.001f, "invalid water height");
    for (const auto& d : r.drops)
        Check(std::isfinite(d.x) && std::isfinite(d.y) && std::isfinite(d.vx) && std::isfinite(d.vy), "invalid particle");
}
static Drop Falling(float x, float y, float vy) {
    Drop d; d.x=x; d.y=y; d.vy=vy; d.z=1; d.mass=8; d.size=2; return d;
}
int main() {
    int passed = 0, failed = 0;
    auto test = [&](const char* name, std::function<void()> fn) {
        Reset();
        try { fn(); ++passed; std::printf("PASS %s\n", name); }
        catch (const std::exception& e) { ++failed; std::printf("FAIL %s: %s\n", name, e.what()); }
    };
    test("presets and explicit lightning override", [] {
        for (int i=0; i<4; ++i) {
            ApplyPreset((Preset)i); Check(CurrentPreset()==i,"preset mismatch");
            Check(Cfg().lightning == (i==3), "storm lightning mismatch");
        }
        Cfg().lightning=false; Check(CurrentPreset()==3,"manual lightning override lost profile");
        int old=Cfg().intensity; ApplyPreset((Preset)99); Check(Cfg().intensity==old,"invalid preset changed config");
    });
    test("reset retains language and enabled state", [] {
        Cfg().lang=4; Cfg().enabled=false; Cfg().lightning=true; ResetSettings();
        Check(Cfg().lang==4 && !Cfg().enabled && !Cfg().lightning,"reset state incorrect");
    });
    test("rain accumulates on both windows and taskbar", [] {
        auto w=Desktop(); Window(w,1,{250,350,950,850}); Rain r; r.Sync(w); Step(r,w,1800);
        Check(Volume(Find(r,1))>10,"window stayed dry");
        Check(r.surfaces.size()==2 && Volume(r)>Volume(Find(r,1)),"taskbar stayed dry"); Finite(r);
    });
    test("swept collision chooses nearest edge at 15 FPS", [] {
        Cfg().intensity=0; Cfg().angle=0; auto w=Desktop();
        Window(w,1,{200,300,800,500}); Window(w,2,{200,550,800,850});
        Rain r; r.Sync(w); r.drops.push_back(Falling(400,245,1800)); r.Update(1.0f/15,w,{0,0},false);
        Check(Volume(Find(r,1))>1,"upper edge was tunneled through");
        Check(Volume(Find(r,2))<0.001f,"drop hit wrong edge"); Finite(r);
    });
    test("water follows a slow window move", [] {
        Cfg().intensity=0; auto w=Desktop(); Window(w,1,{100,300,900,700}); Rain r; r.Sync(w);
        auto& s=Find(r,1); std::fill(s.h.begin(),s.h.end(),3); Step(r,w,60);
        float before=Volume(Find(r,1)); w.wins[0].rc={200,340,1000,740}; r.Sync(w);
        Check(std::fabs(Volume(Find(r,1))-before)<0.05f,"slow move lost water");
        Check(Find(r,1).left==207 && Find(r,1).top==340,"surface did not follow");
    });
    test("fast window shake ejects water", [] {
        Cfg().intensity=0; auto w=Desktop(); Window(w,1,{100,300,900,700}); Rain r; r.Sync(w);
        std::fill(Find(r,1).h.begin(),Find(r,1).h.end(),5); float before=Volume(Find(r,1));
        w.wins[0].rc={500,300,1300,700}; r.Sync(w);
        Check(Volume(Find(r,1))<before && !r.drops.empty(),"shake did not eject droplets");
    });
    test("closed or minimized window loses its surface", [] {
        auto w=Desktop(); Window(w,1,{100,300,900,700}); Rain r; r.Sync(w); w.wins.clear(); r.Sync(w);
        Check(r.surfaces.size()==1 && r.surfaces[0].ground,"stale window surface");
    });
    test("overlap computes correct visibility band", [] {
        auto w=Desktop(); Window(w,1,{200,200,800,800}); Window(w,2,{100,400,1000,900});
        Rain r; r.Sync(w); auto& s=Find(r,2);
        int cell=(400-s.left)/kCell; Check(s.hideBelow[cell]>=200,"foreground window not occluding back edge");
        Check(s.hideBelow[0]==0,"uncovered edge was hidden");
    });
    test("water diffusion conserves volume without evaporation", [] {
        Cfg().intensity=0; auto w=Desktop(); Rain r; r.Sync(w); auto& s=r.surfaces[0];
        s.h[s.cells/2]=20; float before=Volume(r); Step(r,w,120);
        Check(std::fabs(Volume(r)-before)<0.02f,"diffusion changed volume"); Finite(r);
    });
    test("evaporation reaches zero without negative heights", [] {
        Cfg().intensity=0; Cfg().evaporation=100; auto w=Desktop(); Rain r; r.Sync(w);
        std::fill(r.surfaces[0].h.begin(),r.surfaces[0].h.end(),1); Step(r,w,180);
        Check(Volume(r)<0.01f,"water did not evaporate"); Finite(r);
    });
    test("zero accumulation preserves splash-only rainfall", [] {
        Cfg().accumulation=0; auto w=Desktop(); Rain r; r.Sync(w); Step(r,w,600);
        Check(Volume(r)<0.01f && !r.drops.empty(),"zero accumulation not honored");
    });
    test("hiding falling rain preserves water simulation", [] {
        auto w=Desktop(); Rain a; a.Sync(w); Step(a,w,300); float v=Volume(a);
        Reset(); Cfg().showFalling=false; Rain b; b.Sync(w); Step(b,w,300);
        Check(std::fabs(v-Volume(b))<0.01f && v>0,"water-only mode differs");
    });
    test("cursor displaces pooled water into droplets", [] {
        Cfg().intensity=0; auto w=Desktop(); Rain r; r.Sync(w); auto& s=r.surfaces[0];
        std::fill(s.h.begin(),s.h.end(),20); float before=Volume(r);
        r.Update(1.0f/60,w,{400,1032},true);
        Check(Volume(r)<before && !r.drops.empty(),"cursor did not splash water"); Finite(r);
    });
    test("window runoff creates droplets and stays bounded", [] {
        Cfg().intensity=0; auto w=Desktop(); Window(w,1,{200,300,900,800}); Rain r; r.Sync(w);
        std::fill(Find(r,1).h.begin(),Find(r,1).h.end(),8); float before=Volume(Find(r,1)); Step(r,w,15);
        Check(Volume(Find(r,1))<before && !r.drops.empty(),"runoff missing"); Finite(r);
    });
    test("disabled surfaces and zero depth are respected", [] {
        auto w=Desktop(); Window(w,1,{200,300,900,800}); Rain r;
        Cfg().onWindows=false; Cfg().onTaskbar=false; r.Sync(w); Check(r.surfaces.empty(),"disabled surfaces remain");
        Cfg().onWindows=true; Cfg().onTaskbar=true; Cfg().waterTaskbar=0; Cfg().waterWindows=0;
        r.Sync(w); Step(r,w,600); Check(Volume(r)<0.001f,"zero water depth ignored"); Finite(r);
    });
    test("lighting is completely disabled by its switch", [] {
        auto w=Desktop(); Rain r; r.FlashNow(w); Check(r.bolt.segments.empty(),"disabled manual strike occurred");
        Cfg().lightning=true; r.FlashNow(w); Check(!r.bolt.segments.empty() && r.BoltOpacity()>0,"manual strike missing");
        Cfg().lightning=false; Check(r.BoltOpacity()==0,"disabled bolt still visible"); Step(r,w,1);
        Check(r.bolt.segments.empty(),"disabled bolt was not cleared");
    });
    test("lightning branches stay bounded and afterglow ends", [] {
        Cfg().lightning=true; Cfg().lightningBranches=100; auto w=Desktop(); Rain r; r.FlashNow(w);
        Check(r.bolt.segments.size()==192,"unexpected branch segment count");
        for (auto s:r.bolt.segments) Check(std::isfinite(s.x1)&&std::isfinite(s.y2)&&s.width>0,"invalid bolt geometry");
        Cfg().lightningInterval=120; Step(r,w,60); Check(r.BoltOpacity()==0,"afterglow never ended");
    });
    test("automatic strikes work without falling rain", [] {
        Cfg().intensity=0; Cfg().lightning=true; Cfg().lightningInterval=8;
        auto w=Desktop(); Rain r; r.Sync(w); int strikes=0; bool prev=false;
        for (int i=0;i<2400;++i) { Step(r,w,1); bool active=r.BoltOpacity()>0; if(active&&!prev)++strikes; prev=active; }
        Check(strikes>=3 && !r.Idle(),"independent lightning scheduler not active");
    });
    test("mixed DPI layouts remain finite at maximum settings", [] {
        Cfg().intensity=100; Cfg().fallSpeed=100; Cfg().angle=60; Cfg().gusts=100;
        Cfg().accumulation=100; Cfg().waterWindows=100; Cfg().waterTaskbar=200; Cfg().lightning=true;
        auto w=Desktop(5760,2160);
        w.monitors[0].rc={0,0,1920,1080}; w.monitors[0].groundY=1040;
        MonitorInfo m; m.rc={1920,0,5760,2160}; m.work={1920,0,5760,2080}; m.groundY=2080; m.scale=2; w.monitors.push_back(m);
        Window(w,1,{200,300,1700,900}); Window(w,2,{2200,600,5000,1800}); Rain r; r.Sync(w);
        for (int i=0;i<1600;++i) {
            float dt=i%2 ? 1.0f/15 : 1.0f/144;
            r.Update(dt,w,{i%5760,800},true); if(i%45==0)r.Sync(w); if(i%100==0)Finite(r);
        }
        Finite(r); Check(Volume(r)>0,"stress layout accumulated no water");
    });
    test("clear removes all transient effects", [] {
        Cfg().lightning=true; auto w=Desktop(); Rain r; r.Sync(w); Step(r,w,300); r.FlashNow(w); r.ClearAll();
        Check(r.drops.empty() && r.splashes.empty() && r.bolt.segments.empty() && Volume(r)==0,"clear left effects");
    });
    test("line clipping splits at a foreground window", [] {
        auto w=Desktop(); Window(w,1,{40,20,60,80});
        auto spans=VisibleLine(0,50,100,50,0,w);
        Check(spans.size()==2 && std::fabs(spans[0].second-0.4f)<0.001f && std::fabs(spans[1].first-0.6f)<0.001f,"incorrect visible intervals");
    });
    test("fully occluded lines disappear", [] {
        auto w=Desktop(); Window(w,1,{0,0,100,100});
        Check(VisibleLine(20,20,80,80,0,w).empty(),"covered bolt leaked through window");
    });
    test("vertical and diagonal clipping", [] {
        auto w=Desktop(); Window(w,1,{40,40,60,60});
        auto a=VisibleLine(50,0,50,100,0,w),b=VisibleLine(0,0,100,100,0,w);
        Check(a.size()==2 && b.size()==2 && std::fabs(a[0].second-0.4f)<0.001f && std::fabs(b[1].first-0.6f)<0.001f,"clipping direction bug");
    });
    test("overlapping windows merge covered line intervals", [] {
        auto w=Desktop(); Window(w,1,{20,0,60,100}); Window(w,2,{40,0,80,100});
        auto a=VisibleLine(0,50,100,50,0,w);
        Check(a.size()==2 && std::fabs(a[0].second-0.2f)<0.001f && std::fabs(a[1].first-0.8f)<0.001f,"overlap exposed hidden rain");
    });
    test("line width prevents glowing edges leaking into windows", [] {
        auto w=Desktop(); Window(w,1,{40,20,60,80}); auto a=VisibleLine(0,50,100,50,4,w);
        Check(a.size()==2 && std::fabs(a[0].second-0.36f)<0.001f && std::fabs(a[1].first-0.64f)<0.001f,"stroke padding ignored");
    });
    test("desktop illumination respects window area", [] {
        auto w=Desktop(100,100); Window(w,1,{25,25,75,75});
        auto rects=VisibleDesktopRects(w.virt,w);int area=0;
        for(auto r:rects)area+=r.w()*r.h();
        Check(area==7500 && rects.size()==4,"desktop flash covers foreground window");
    });
    test("fullscreen occlusion removes all desktop illumination", [] {
        auto w=Desktop(100,100); Window(w,1,{-10,-10,110,110});
        Check(VisibleDesktopRects(w.virt,w).empty(),"fullscreen not fully excluded");
    });
    test("empty desktop keeps the whole effect visible", [] {
        auto w=Desktop(); auto a=VisibleLine(0,0,100,100,10,w);
        Check(a.size()==1 && a[0].first==0 && a[0].second==1,"empty desktop line clipped");
        auto b=VisibleDesktopRects(w.virt,w);Check(b.size()==1 && b[0].w()==1920,"empty desktop flash clipped");
    });
    std::printf("\n%d passed, %d failed\n",passed,failed);
    return failed ? 1 : 0;
}
