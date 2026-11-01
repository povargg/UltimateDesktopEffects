#include "rain.h"

namespace rainfx {
// Drifts: binding to windows, occlusion, sliding off, cursor interaction

static constexpr uintptr_t kGroundKeyBase = ~(uintptr_t)0 - 64;

void Rain::InitSurface(Surface& s, uintptr_t key, bool ground, int left, int top, int width) {
    s.key = key;
    s.ground = ground;
    s.left = left;
    s.top = top;
    s.width = width;
    s.cells = std::max(1, width / kCell);
    s.h.assign(s.cells, 0.0f);
    s.spillL = s.spillR = 0;
    Shape(s);
}

// Water levels horizontally; the waves setting affects rendering, not permanent hills.
void Rain::Shape(Surface& s) {
    s.rough = Cfg().waves / 100.0f;
    s.cap.assign(s.cells, 1.0f);
    s.dep.assign(s.cells, 1.0f);
    s.slope.assign(s.cells, 0.0f);
    s.smooth.assign(s.cells, 0.5f);
}

void Rain::ComputeOcclusion(Surface& s, const World& w, int zLimit) {
    s.hideBelow.assign(s.cells, 0.0f);
    s.visCeil.assign(s.cells, 1e9f);
    const int y0 = s.top - 1;  // pixel right above the edge, where the water lies
    for (int k = 0; k < zLimit && k < (int)w.wins.size(); ++k) {
        const RectI& o = w.wins[k].rc;
        if (o.r <= s.left || o.l >= s.left + s.width) continue;
        int c0 = std::max(0, (o.l - s.left) / kCell);
        int c1 = std::min(s.cells, (o.r - s.left + kCell - 1) / kCell);
        if (o.t <= y0 && o.b > y0) {
            // A window in front covers the edge: snow above its top edge still shows
            float below = (float)(s.top - o.t);
            for (int c = c0; c < c1; ++c) s.hideBelow[c] = std::max(s.hideBelow[c], below);
        } else if (o.b <= y0) {
            float ceil = (float)(s.top - o.b);
            for (int c = c0; c < c1; ++c) s.visCeil[c] = std::min(s.visCeil[c], ceil);
        }
    }
}

float Rain::ScaleAt(float x, float y) const {
    const MonitorInfo* best = nullptr;
    float bestDist = 1e9f;
    for (const auto& m : mons_) {
        float dx = x < m.rc.l ? m.rc.l - x : (x >= m.rc.r ? x - m.rc.r : 0);
        float dy = y < m.rc.t ? m.rc.t - y : (y >= m.rc.b ? y - m.rc.b : 0);
        if (dx + dy < bestDist) { bestDist = dx + dy; best = &m; }
    }
    return best ? best->scale : scale_;
}

void Rain::Sync(const World& w) {
    const Settings& c = Cfg();
    mons_ = w.monitors;
    const bool behind = c.mode == MODE_BEHIND;
    std::vector<Surface> next;
    next.reserve(surfaces.size() + 4);

    // Average water on windows: popups such as Start and menus start with that much
    float avgPile = 0;
    int pileCount = 0;
    for (const auto& s : surfaces) {
        if (s.ground || s.cells == 0) continue;
        float sum = 0;
        for (float v : s.h) sum += v;
        if (sum > 0) { avgPile += sum / s.cells; ++pileCount; }
    }
    if (pileCount) avgPile /= pileCount;

    auto take = [&](uintptr_t key, bool ground, int left, int top, int width, float maxH, int zLimit,
                    int anchorL, int anchorR, float sc, bool transient) {
        Surface* old = nullptr;
        for (auto& s : surfaces)
            if (s.key == key) { old = &s; break; }
        Surface s;
        if (!old) {
            InitSurface(s, key, ground, left, top, width);
            if (transient && avgPile > 0) {  // Start and menus live for seconds: give them a light cap right away
                float level = Clampf(avgPile, 3 * sc, maxH * 0.4f);
                float wt = Clampf(maxH * 1.2f, 12.0f * sc, 130.0f * sc);
                for (int c = 0; c < s.cells; ++c) {
                    float t = std::min(1.0f, std::min(c + 0.5f, s.cells - c - 0.5f) * kCell / wt);
                    s.h[c] = level * (0.6f + 0.4f * s.cap[c]) * t * t * (3 - 2 * t);
                }
            }
        } else if (old->left == left && old->top == top && old->width == width) {
            s = std::move(*old);
        } else {
            Follow(s, *old, left, top, width, anchorL, anchorR);  // the window is moving: its water rides along
        }
        if (old) old->key = 0;
        s.anchorL = anchorL;
        s.anchorR = anchorR;
        if (s.rough != c.waves / 100.0f) Shape(s);
        s.maxH = maxH;
        s.scale = sc;
        s.lastSync = time_;
        s.z = zLimit;
        ComputeOcclusion(s, w, zLimit);
        next.push_back(std::move(s));
    };

    const int virtW = w.virt.w();
    if (c.onTaskbar) {
        int zGround = behind ? (int)w.wins.size() : std::max(0, w.firstTaskbarZ);
        for (size_t i = 0; i < w.monitors.size(); ++i) {
            const MonitorInfo& m = w.monitors[i];
            take(kGroundKeyBase + i, true, m.rc.l, m.groundY, m.rc.w(), c.waterTaskbar * m.scale, zGround, m.rc.l, m.rc.r,
                 m.scale, false);
        }
    }
    if (c.onWindows) {
        for (size_t z = 0; z < w.wins.size(); ++z) {
            const WinInfo& wi = w.wins[z];
            if (wi.taskbar) continue;
            const MonitorInfo* mon = nullptr;
            int cx = (wi.rc.l + wi.rc.r) / 2;
            for (auto& m : w.monitors)
                if (cx >= m.rc.l && cx < m.rc.r && wi.rc.t >= m.rc.t && wi.rc.t < m.rc.b) { mon = &m; break; }
            if (!mon) continue;
            if (wi.rc.t <= mon->rc.t + 2 || wi.rc.t >= mon->groundY - 2) continue;  // maximized, or below the taskbar
            const int inset = (int)(7 * mon->scale);  // Windows 11 rounded corners
            int left = std::max(0, wi.rc.l + inset);
            int right = std::min(virtW, wi.rc.r - inset);
            if (right - left < 40) continue;
            float maxH = std::min(c.waterWindows * mon->scale, (float)(wi.rc.t - mon->rc.t));
            take((uintptr_t)wi.hwnd, false, left, wi.rc.t, right - left, maxH, (int)z, wi.rc.l, wi.rc.r, mon->scale,
                 wi.transient);
        }
    }
    // Windows that are gone (closed or minimized): their snow just disappears
    surfaces = std::move(next);
}

void Rain::SpawnChunk(float x, float y, float vx, float vy, float mass) {
    if (drops.size() >= 7000) return;
    Drop f{};
    f.x = x; f.y = y; f.vx = vx; f.vy = vy;
    f.mass = mass;
    f.size = Clampf(std::sqrt(mass) * 0.55f, 0.8f, 5.0f * ScaleAt(x, y));
    f.z = 1.0f;
    f.alpha = 0.95f;
    f.kind = DROP_SPLASH;
    f.s = ScaleAt(x, y);
    f.life = 3.0f;
    drops.push_back(f);
}

void Rain::ShakeOff(const Surface& s, int maxChunks) {
    double total = 0;
    for (float v : s.h) total += v;
    total *= kCell;
    if (total < 1) return;
    int count = (int)std::clamp(total / ChunkMass(s.scale), 1.0, (double)maxChunks);
    float mass = (float)(total / count);
    float acc = Rand01() * mass;
    for (int c = 0; c < s.cells; ++c) {
        acc += s.h[c] * kCell;
        while (acc >= mass) {
            acc -= mass;
            SpawnChunk(s.left + (c + Rand01()) * kCell, s.top - s.h[c] * Rand01(),
                       RandRange(-30, 30) * s.scale, RandRange(-30, 30) * s.scale, mass);
        }
    }
}

// Shake off a share of the water (fast window move). vx, vy: initial clump velocity
void Rain::ShakePart(Surface& s, float frac, float vx, float vy) {
    const float m = ChunkMass(s.scale);
    float acc = 0;
    for (int c = 0; c < s.cells; ++c) {
        float take = s.h[c] * frac;
        if (take <= 0) continue;
        s.h[c] -= take;
        acc += take * kCell;
        while (acc >= m) {
            acc -= m;
            SpawnChunk(s.left + (c + Rand01()) * kCell, s.top - s.h[c] - Rand01() * take,
                       vx + RandRange(-40, 40) * s.scale, vy - RandRange(0, 60) * s.scale, m);
        }
    }
}

// Window moved or resized: water stays on it, the excess falls off
void Rain::Follow(Surface& s, Surface& old, int left, int top, int width, int anchorL, int anchorR) {
    float dt = std::max(0.008f, time_ - old.lastSync);
    // Same real window width means a move, even if the screen edge clips the window
    const bool moved = anchorR - anchorL == old.anchorR - old.anchorL;
    const int shift = moved ? anchorL - old.anchorL : 0;
    float dx = (float)shift, dy = (float)(top - old.top);
    if (moved && old.width == width) {
        s = std::move(old);
        s.left = left;
        s.top = top;
    } else {
        // Rain moves with the window (or stays put on resize),
        // and whatever ends up past the edge falls
        InitSurface(s, old.key, old.ground, left, top, width);
        const float m = ChunkMass(old.scale);
        float acc = 0;
        if (!moved) dy = 0;
        for (int oc = 0; oc < old.cells; ++oc) {
            float x = old.left + (oc + 0.5f) * kCell;
            int c = (int)std::floor((x + shift - left) / kCell);
            if (c >= 0 && c < s.cells) {
                s.h[c] = std::max(s.h[c], old.h[oc]);
                continue;
            }
            acc += old.h[oc] * kCell;
            while (acc >= m) {
                acc -= m;
                SpawnChunk(x + shift, top - old.h[oc] * Rand01(), RandRange(-20, 20) * old.scale, 0, m);
            }
        }
    }
    // Slow drag: snow rides along. Fast shake: some of it flies off
    float speed = std::sqrt(dx * dx + dy * dy) / dt;
    float frac = Clampf((speed - 900 * old.scale) / (6000 * old.scale), 0.0f, 0.35f);
    if (frac > 0) ShakePart(s, frac, dx / dt * 0.3f, dy / dt * 0.3f);
}

void Rain::ShakeAll() {
    for (auto& s : surfaces) {
        ShakeOff(s, 700);
        std::fill(s.h.begin(), s.h.end(), 0.0f);
        s.spillL = s.spillR = 0;
    }
}

void Rain::ClearAll() {
    drops.clear();
    for (auto& s : surfaces) { std::fill(s.h.begin(), s.h.end(), 0.0f); s.spillL = s.spillR = 0; }
    splashes.clear();
    bolt.segments.clear();
    prefilled_ = false;
}

void Rain::Land(const Drop& f, Surface& s, int cell) {
    float area = f.kind == DROP_SPLASH ? f.mass : f.mass * accumK_;
    int spread = std::clamp((int)(f.size * 2 / kCell) + 2, 2, 12);
    float wsum = 0;
    for (int d = -spread; d <= spread; ++d)
        if (cell + d >= 0 && cell + d < s.cells) wsum += spread + 1 - std::abs(d);
    for (int d = -spread; d <= spread; ++d) {
        int i = cell + d;
        if (i < 0 || i >= s.cells) continue;
        float add = area / kCell * (spread + 1 - std::abs(d)) / std::max(1.0f, wsum);
        float cap = std::max(0.0f, s.maxH);
        float excess = std::max(0.0f, s.h[i] + add - cap);
        s.h[i] = std::min(cap, s.h[i] + add);
        if (!s.ground) {
            if (i < s.cells / 2) s.spillL += excess * kCell;
            else s.spillR += excess * kCell;
        }
    }
    if (f.kind != DROP_SPLASH && Cfg().splashes > 0 && splashes.size() < 1200 &&
        Rand01() * 100 < Cfg().splashes) {
        splashes.push_back({f.x, f.y, 0, RandRange(0.25f, 0.5f), RandRange(6, 15) * f.s, f.s, s.key});
    }
}

void Rain::Relax(Surface& s, float dt) {
    const int n = s.cells;
    if (n < 2) return;
    auto& h = s.h;
    // Conservative, stable pairwise diffusion. Alternate direction to avoid a left/right bias.
    const float k = std::min(0.24f, dt * 8.0f);
    for (int pass = 0; pass < 4; ++pass) {
        relaxDir_ = !relaxDir_;
        for (int j = 0; j + 1 < n; ++j) {
            int i = relaxDir_ ? j : n - 2 - j;
            float flow = (h[i] - h[i + 1]) * k;
            h[i] -= flow; h[i + 1] += flow;
        }
    }
    float evap = Cfg().evaporation * 0.008f * s.scale * dt;
    for (auto& v : h) v = std::max(0.0f, std::min(v, s.maxH) - evap);
    if (s.ground) return;
    // Open window edges continuously drain, producing drops that can land on lower windows.
    int edge = std::min(n / 2, std::max(1, (int)(6 * s.scale / kCell)));
    float drain = std::min(1.0f, dt * 2.5f);
    for (int i = 0; i < edge; ++i) {
        float weight = 1.0f - (float)i / edge;
        float l = h[i] * drain * weight, r = h[n - 1 - i] * drain * weight;
        h[i] -= l; h[n - 1 - i] -= r;
        s.spillL += l * kCell; s.spillR += r * kCell;
    }
    float mass = ChunkMass(s.scale);
    for (int budget = 0; budget < 24 && s.spillL >= mass; ++budget) {
        s.spillL -= mass; SpawnSlide(s, -1, mass);
    }
    for (int budget = 0; budget < 24 && s.spillR >= mass; ++budget) {
        s.spillR -= mass; SpawnSlide(s, 1, mass);
    }
}

void Rain::SpawnSlide(const Surface& s, int dir, float mass) {
    float x = dir < 0 ? s.left - 2.0f * s.scale : s.left + s.width + 2.0f * s.scale;
    SpawnChunk(x, (float)s.top, dir * RandRange(12, 38) * s.scale, RandRange(30, 80) * s.scale, mass);
}

bool Rain::MoveSlide(Drop& f, float dt) {
    const Surface* s = nullptr;
    for (const auto& c : surfaces) if (c.key == f.surf) { s = &c; break; }
    if (!s || f.age >= f.life) return false;
    f.vx = f.dir * std::min(std::fabs(f.vx) + 500 * f.s * dt, 450 * f.s);
    f.x += f.vx * dt;
    if (f.x < s->left || f.x >= s->left + s->width) return false;
    int cell = std::clamp((int)((f.x - s->left) / kCell), 0, s->cells - 1);
    f.y = s->top - s->h[cell];
    return true;
}

void Rain::Knock(float dt, POINT cur) {
    const float sc = ScaleAt((float)cur.x, (float)cur.y);
    const float r = Cfg().cursorRadius * sc;
    const float m = ChunkMass(sc);
    for (auto& s : surfaces) {
        if (cur.x + r < s.left || cur.x - r > s.left + s.width) continue;
        if (cur.y > s.top + 2 || cur.y < s.top - s.maxH - r) continue;
        int c0 = std::max(0, (int)((cur.x - r - s.left) / kCell));
        int c1 = std::min(s.cells - 1, (int)((cur.x + r - s.left) / kCell));
        float acc = 0;
        for (int c = c0; c <= c1; ++c) {
            const float hb = s.hideBelow[c];
            if (s.h[c] <= hb + 0.05f) continue;  // no visible snow in this cell
            float cx = s.left + (c + 0.5f) * kCell;
            float dx = cx - cur.x;
            if (std::fabs(dx) >= r) continue;
            float span = std::sqrt(r * r - dx * dx);
            if (cur.y - span >= s.top - hb) continue;  // cursor is on the window in front, below the visible snow
            float newH = Clampf(s.top - (cur.y + span), hb, s.h[c]);
            float removed = s.h[c] - newH;
            if (removed < 0.05f) continue;
            s.h[c] = newH;
            acc += removed * kCell;
            while (acc >= m) {
                acc -= m;
                SpawnChunk(cx, s.top - newH - Rand01() * removed,
                           curVx_ * 0.35f + RandRange(-50, 50) * sc,
                           curVy_ * 0.35f - RandRange(40, 170) * sc, m);
            }
        }
    }
    (void)dt;
}
} // namespace rainfx
