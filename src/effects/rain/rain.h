#pragma once
#include "../../common.h"
#include "settings.h"
#include "../../world.h"

namespace rainfx {

constexpr int kCell = 2;
enum DropKind : uint8_t { DROP_STREAK, DROP_LARGE, DROP_SPLASH, DROP_RUNOFF };
struct Drop {
    float x = 0, y = 0, vx = 0, vy = 0;
    float size = 1, z = 1, phase = 0, freq = 1;
    float rot = 0, vrot = 0, alpha = 1, mass = 1;
    DropKind kind = DROP_STREAK;
    float s = 1, age = 0, life = 8;
    int8_t dir = 0;
    uintptr_t surf = 0;
};
// Height per two-pixel cell represents volume. Visual waves do not change volume.
struct Surface {
    uintptr_t key = 0;
    bool ground = false;
    int left = 0, top = 0, width = 0, cells = 0;
    float maxH = 0, scale = 1;
    std::vector<float> h, cap, dep, slope, smooth;
    float rough = -1;
    std::vector<float> hideBelow, visCeil;
    int z = 0;
    float spillL = 0, spillR = 0, lastSync = 0;
    int anchorL = 0, anchorR = 0;
};
struct Splash {
    float x, y, age, life, size, scale;
    uintptr_t surface;
};
struct BoltSegment { float x1, y1, x2, y2, width; };
struct LightningBolt {
    std::vector<BoltSegment> segments;
    RectI monitor;
    float age = 1, duration = 0.55f;
};
class Rain {
public:
    std::vector<Drop> drops;
    std::vector<Surface> surfaces;
    std::vector<Splash> splashes;
    LightningBolt bolt;
    void Sync(const World& w);
    void Update(float dt, const World& w, POINT cursor, bool cursorValid);
    void ShakeAll();
    void ClearAll();
    bool Idle() const;
    bool VisibleMotion() const;
    void SetScale(float s) { scale_ = s; }
    float Time() const { return time_; }
    float BoltOpacity() const;
    void FlashNow(const World& w);
private:
    void Spawn(float dt, const World& w);
    void SpawnDrop(const World& w, bool anywhere);
    void SpawnChunk(float x, float y, float vx, float vy, float mass);
    void SpawnSlide(const Surface& s, int dir, float mass);
    bool MoveSlide(Drop& f, float dt);
    void Land(const Drop& f, Surface& s, int cell);
    void Relax(Surface& s, float dt);
    void Knock(float dt, POINT cur);
    void ShakeOff(const Surface& s, int maxChunks);
    void ShakePart(Surface& s, float frac, float vx, float vy);
    void Follow(Surface& s, Surface& old, int left, int top, int width, int anchorL, int anchorR);
    void InitSurface(Surface& s, uintptr_t key, bool ground, int left, int top, int width);
    void Shape(Surface& s);
    void ComputeOcclusion(Surface& s, const World& w, int zLimit);
    void UpdateLightning(float dt, const World& w);
    static float ChunkMass(float sc) { return 4.0f * sc * sc; }
    float ScaleAt(float x, float y) const;
    float scale_ = 1;
    std::vector<MonitorInfo> mons_;
    float time_ = 0, angleDeg_ = 0, cursorAngle_ = 0, spawnAcc_ = 0, accumK_ = 1;
    float curVx_ = 0, curVy_ = 0;
    POINT prevCursor_ = {0, 0};
    bool havePrevCursor_ = false, prefilled_ = false, relaxDir_ = false;
    float nextLightning_ = 0;
    bool wasLightning_ = false;
};
} // namespace rainfx
