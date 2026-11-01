#include "rain.h"

namespace rainfx {
// Rain simulation; surface ownership is derived from LayerSnow (MIT).
static constexpr float kPi = 3.14159265f;
static constexpr size_t kMaxDrops = 7000;
static float SpeedBase(float scale) { return (320.0f + Cfg().fallSpeed * 9.0f) * scale; }
static float MonitorTarget(const MonitorInfo& m) {
    return Cfg().intensity * 10.0f * m.LogicalArea() / (1920.0f * 1080.0f);
}
void Rain::SpawnDrop(const World& w, bool anywhere) {
    if (drops.size() >= kMaxDrops) return;
    const Settings& c = Cfg();
    RectI mr = {0, 0, w.virt.w(), w.virt.h()};
    float sc = scale_, total = 0;
    for (const auto& m : w.monitors) total += m.LogicalArea();
    float pick = Rand01() * total;
    for (const auto& m : w.monitors) {
        mr = m.rc; sc = m.scale; pick -= m.LogicalArea();
        if (pick <= 0) break;
    }
    Drop f;
    f.s = sc; f.z = 0.35f + 0.65f * std::pow(Rand01(), 1.3f);
    f.size = (0.65f + c.dropSize * 0.021f) * sc * (0.55f + 0.65f * f.z) * RandRange(0.8f, 1.2f);
    f.mass = f.size * f.size * 1.2f; f.alpha = 0.3f + 0.65f * f.z;
    if (Rand01() * 100 < c.largeDropShare) {
        f.kind = DROP_LARGE; f.size *= 1.45f; f.mass *= 1.7f;
    }
    float tanA = std::tan(angleDeg_ * kPi / 180.0f), drift = mr.h() * tanA;
    f.x = RandRange(std::min((float)mr.l, mr.l - drift) - 20, std::max((float)mr.r, mr.r - drift) + 20);
    f.y = anywhere ? RandRange(mr.t - 20.0f, mr.t + mr.h() * 0.95f) : mr.t - 30.0f;
    f.vy = SpeedBase(sc) * (0.55f + 0.45f * f.z); f.vx = f.vy * tanA;
    f.phase = Rand01() * 2 * kPi; f.freq = RandRange(0.6f, 1.8f);
    f.life = std::max(3.0f, mr.h() / std::max(1.0f, f.vy) + 2.0f);
    drops.push_back(f);
}
void Rain::Spawn(float dt, const World& w) {
    float target = 0, rate = 0;
    for (const auto& m : w.monitors) {
        float t = MonitorTarget(m); target += t;
        rate += t * SpeedBase(m.scale) * 0.8f / std::max(40, m.rc.h());
    }
    if (target > kMaxDrops * 0.75f) {
        rate *= kMaxDrops * 0.75f / target; target = kMaxDrops * 0.75f;
    }
    if (target < 1) { spawnAcc_ = 0; prefilled_ = false; return; }
    if (!prefilled_) {
        for (int i = 0; i < (int)(target * 0.45f); ++i) SpawnDrop(w, true);
        prefilled_ = true;
    }
    size_t live = 0;
    for (const auto& f : drops) live += f.kind == DROP_STREAK || f.kind == DROP_LARGE;
    spawnAcc_ = std::min(spawnAcc_ + rate * dt, 300.0f);
    while (spawnAcc_ >= 1 && live < target * 1.25f && drops.size() < kMaxDrops) {
        SpawnDrop(w, false); spawnAcc_ -= 1; ++live;
    }
}
void Rain::Update(float dt, const World& w, POINT cur, bool cursorValid) {
    dt = Clampf(dt, 0, 0.1f);
    const Settings& c = Cfg(); time_ += dt;
    accumK_ = c.accumulation <= 0 ? 0 : std::pow(c.accumulation / 50.0f, 1.6f);
    if (cursorValid && havePrevCursor_ && dt > 0) {
        float k = std::min(1.0f, dt * 12);
        curVx_ += ((cur.x - prevCursor_.x) / dt - curVx_) * k;
        curVy_ += ((cur.y - prevCursor_.y) / dt - curVy_) * k;
    } else curVx_ = curVy_ = 0;
    prevCursor_ = cur; havePrevCursor_ = cursorValid;
    float cursorTarget = 0;
    if (cursorValid && c.cursorWind > 0) for (const auto& m : w.monitors) {
        if (!m.rc.contains((float)cur.x, (float)cur.y)) continue;
        float nx = (cur.x - (m.rc.l + m.rc.r) * 0.5f) / std::max(1.0f, m.rc.w() * 0.5f);
        cursorTarget = Clampf(nx, -1, 1) * c.cursorWind * 0.4f; break;
    }
    cursorAngle_ += (cursorTarget - cursorAngle_) * std::min(1.0f, dt * 1.2f);
    angleDeg_ = Clampf(c.angle + cursorAngle_, -75, 75);
    Spawn(dt, w);
    if (cursorValid && c.cursorKnock) Knock(dt, cur);
    float speed = SpeedBase(1), tanA = std::tan(angleDeg_ * kPi / 180.0f);
    float gust = c.gusts * 0.0035f * speed *
        (std::sin(time_ * 0.31f) * 0.65f + std::sin(time_ * 0.87f + 1.7f) * 0.35f);
    float relaxK = std::min(1.0f, dt * 8);
    float pushR = c.cursorRadius * ScaleAt((float)cur.x, (float)cur.y) * 2.2f;
    bool push = cursorValid && c.cursorWind > 0 && curVx_ * curVx_ + curVy_ * curVy_ > 900;
    for (size_t i = 0; i < drops.size();) {
        Drop& f = drops[i]; f.age += dt;
        if (f.kind == DROP_RUNOFF && MoveSlide(f, dt)) { ++i; continue; }
        if (f.kind == DROP_RUNOFF) { f.kind = DROP_SPLASH; f.vy = 40 * f.s; }
        float prevX = f.x, prevY = f.y;
        if (f.kind == DROP_SPLASH) {
            f.vy = std::min(f.vy + 1400 * f.s * dt, 1100 * f.s);
            f.vx *= std::max(0.0f, 1 - dt);
        } else {
            float tvy = speed * f.s * (0.55f + 0.45f * f.z);
            float tvx = tvy * tanA + gust * f.s * (0.45f + 0.55f * f.z);
            f.vx += (tvx - f.vx) * relaxK; f.vy += (tvy - f.vy) * relaxK;
            if (push) {
                float dx = f.x - cur.x, dy = f.y - cur.y, d2 = dx * dx + dy * dy;
                if (d2 < pushR * pushR) {
                    float infl = (1 - std::sqrt(d2) / pushR) * c.cursorWind / 100.0f;
                    f.vx += curVx_ * infl * 0.10f; f.vy += curVy_ * infl * 0.05f;
                }
            }
        }
        f.x += f.vx * dt; f.y += f.vy * dt;
        bool gone = f.y > w.virt.h() + 40 || f.x < -2000 || f.x > w.virt.w() + 2000 || f.age > f.life;
        Surface* hit = nullptr; int hitCell = 0; float firstT = 2;
        if (!gone && f.vy > 0 && (f.z > 0.45f || f.kind == DROP_SPLASH)) for (auto& s : surfaces) {
            if (s.cells == 0) continue;
            float t = Clampf((s.top - prevY) / std::max(0.001f, f.y - prevY), 0, 1);
            float x = prevX + (f.x - prevX) * t;
            if (x < s.left || x >= s.left + s.width) continue;
            int cell = std::clamp((int)((x - s.left) / kCell), 0, s.cells - 1);
            float sy = s.top - s.h[cell];
            if (prevY > sy || f.y < sy) continue;
            t = (sy - prevY) / std::max(0.001f, f.y - prevY);
            x = prevX + (f.x - prevX) * t;
            if (x < s.left || x >= s.left + s.width) continue;
            cell = std::clamp((int)((x - s.left) / kCell), 0, s.cells - 1);
            if (s.h[cell] + 0.5f < s.hideBelow[cell] || s.h[cell] > s.visCeil[cell]) continue;
            if (!s.ground && f.kind != DROP_SPLASH && f.z < 0.7f) continue;
            if (t < firstT) { hit = &s; hitCell = cell; firstT = t; }
        }
        if (hit) {
            Drop landed = f;
            landed.x = prevX + (f.x - prevX) * firstT; landed.y = hit->top - hit->h[hitCell];
            Land(landed, *hit, hitCell); gone = true;
        }
        if (gone) { drops[i] = drops.back(); drops.pop_back(); } else ++i;
    }
    for (auto& s : surfaces) Relax(s, dt);
    for (size_t i = 0; i < splashes.size();) {
        splashes[i].age += dt;
        if (splashes[i].age >= splashes[i].life) { splashes[i] = splashes.back(); splashes.pop_back(); }
        else ++i;
    }
    UpdateLightning(dt, w);
}
float Rain::BoltOpacity() const {
    if (!Cfg().lightning || bolt.segments.empty() || bolt.age >= bolt.duration) return 0;
    return std::exp(-bolt.age * 7.0f) * Clampf((bolt.duration - bolt.age) * 12, 0, 1);
}
static void JaggedPath(std::vector<BoltSegment>& out, float x1, float y1, float x2, float y2,
                       int depth, float jitter, float width) {
    if (depth == 0) { out.push_back({x1, y1, x2, y2, width}); return; }
    float mx = (x1 + x2) * 0.5f + RandRange(-jitter, jitter);
    float my = (y1 + y2) * 0.5f + RandRange(-jitter * 0.13f, jitter * 0.13f);
    JaggedPath(out, x1, y1, mx, my, depth - 1, jitter * 0.52f, width);
    JaggedPath(out, mx, my, x2, y2, depth - 1, jitter * 0.52f, width);
}
void Rain::FlashNow(const World& w) {
    if (!Cfg().enabled || !Cfg().lightning || w.monitors.empty()) return;
    const auto& m = w.monitors[std::min(w.monitors.size() - 1, (size_t)(Rand01() * w.monitors.size()))];
    bolt.segments.clear(); bolt.monitor = m.rc; bolt.age = 0;
    float x = RandRange(m.rc.l + m.rc.w() * 0.18f, m.rc.r - m.rc.w() * 0.18f);
    float endX = Clampf(x + RandRange(-0.23f, 0.23f) * m.rc.w(), (float)m.rc.l + 8, (float)m.rc.r - 8);
    float endY = RandRange(m.rc.t + m.rc.h() * 0.52f, m.rc.t + m.rc.h() * 0.9f);
    JaggedPath(bolt.segments, x, (float)m.rc.t, endX, endY, 6, m.rc.w() * 0.1f, 1.8f * m.scale);
    size_t mainSize = bolt.segments.size();
    for (size_t i = 8; i + 8 < mainSize; i += 6) {
        if (Rand01() * 100 >= Cfg().lightningBranches) continue;
        auto a = bolt.segments[i];
        float bx = Clampf(a.x2 + RandRange(-0.18f, 0.18f) * m.rc.w(), (float)m.rc.l + 4, (float)m.rc.r - 4);
        float by = std::min((float)m.rc.b - 4, a.y2 + RandRange(0.08f, 0.22f) * m.rc.h());
        JaggedPath(bolt.segments, a.x2, a.y2, bx, by, 4, m.rc.w() * 0.025f, 0.85f * m.scale);
    }
    nextLightning_ = Cfg().lightningInterval * RandRange(0.65f, 1.45f);
}
void Rain::UpdateLightning(float dt, const World& w) {
    if (!Cfg().lightning) {
        bolt.segments.clear(); bolt.age = bolt.duration;
        wasLightning_ = false; nextLightning_ = 0; return;
    }
    bolt.age += dt;
    if (bolt.age >= bolt.duration) bolt.segments.clear();
    if (!wasLightning_) { nextLightning_ = Cfg().lightningInterval * RandRange(0.25f, 0.7f); wasLightning_ = true; }
    nextLightning_ = std::min(nextLightning_, Cfg().lightningInterval * 1.45f);
    nextLightning_ -= dt;
    if (nextLightning_ <= 0) FlashNow(w);
}
bool Rain::VisibleMotion() const {
    if (BoltOpacity() > 0 || !splashes.empty()) return true;
    for (const auto& f : drops)
        if (Cfg().showFalling || f.kind == DROP_SPLASH || f.kind == DROP_RUNOFF) return true;
    if (Cfg().waves > 0) for (const auto& s : surfaces) for (float h : s.h) if (h > 0.5f) return true;
    return false;
}
bool Rain::Idle() const {
    if (!drops.empty() || !splashes.empty() || Cfg().lightning) return false;
    for (const auto& s : surfaces) for (float h : s.h) if (h > 0.05f) return false;
    return true;
}
} // namespace rainfx
