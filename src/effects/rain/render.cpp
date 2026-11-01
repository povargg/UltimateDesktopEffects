#include "render.h"
#include "../../occlusion.h"

namespace rainfx {

bool Renderer::Init(HWND hwnd, int width, int height, float scale) {
    Shutdown();
    scale_ = scale;
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0,
                                   D3D11_SDK_VERSION, &d3d_, nullptr, nullptr);
    if (FAILED(hr))
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags, nullptr, 0,
                               D3D11_SDK_VERSION, &d3d_, nullptr, nullptr);
    if (FAILED(hr)) return false;

    ComPtr<IDXGIDevice> dxgiDevice;
    ComPtr<IDXGIFactory2> dxgiFactory;
    if (FAILED(d3d_.As(&dxgiDevice))) return false;
    if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)))) return false;

    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = width;
    sd.Height = height;
    sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.SampleDesc.Count = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    sd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    sd.Scaling = DXGI_SCALING_STRETCH;
    if (FAILED(dxgiFactory->CreateSwapChainForComposition(dxgiDevice.Get(), &sd, nullptr, &swap_))) return false;

    D2D1_FACTORY_OPTIONS fo = {};
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &fo,
                                 reinterpret_cast<void**>(factory_.GetAddressOf()))))
        return false;
    if (FAILED(factory_->CreateDevice(dxgiDevice.Get(), &d2dDevice_))) return false;
    if (FAILED(d2dDevice_->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &dc_))) return false;

    ComPtr<IDXGISurface2> surface;
    if (FAILED(swap_->GetBuffer(0, IID_PPV_ARGS(&surface)))) return false;
    D2D1_BITMAP_PROPERTIES1 bp = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    if (FAILED(dc_->CreateBitmapFromDxgiSurface(surface.Get(), &bp, &target_))) return false;
    dc_->SetTarget(target_.Get());

    if (FAILED(DCompositionCreateDevice(dxgiDevice.Get(), IID_PPV_ARGS(&dcomp_)))) return false;
    if (FAILED(dcomp_->CreateTargetForHwnd(hwnd, TRUE, &dcompTarget_))) return false;
    if (FAILED(dcomp_->CreateVisual(&visual_))) return false;
    visual_->SetContent(swap_.Get());
    dcompTarget_->SetRoot(visual_.Get());
    dcomp_->Commit();

    if (!CreateBrushes()) { Shutdown(); return false; }
    return true;
}

void Renderer::Shutdown() {
    brush_.Reset(); outline_.Reset(); fill_.Reset(); round_.Reset();
    visual_.Reset(); dcompTarget_.Reset(); dcomp_.Reset();
    if (dc_) dc_->SetTarget(nullptr);
    target_.Reset(); dc_.Reset(); d2dDevice_.Reset(); factory_.Reset();
    swap_.Reset(); d3d_.Reset();
}

bool Renderer::CreateBrushes() {
    if (FAILED(dc_->CreateSolidColorBrush(D2D1::ColorF(0.12f, 0.42f, 0.68f, 0.7f), &outline_))) return false;
    if (FAILED(dc_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &brush_))) return false;
    D2D1_GRADIENT_STOP stops[3] = {
        {0, D2D1::ColorF(0.64f, 0.89f, 1.0f, 0.70f)},
        {0.2f, D2D1::ColorF(0.20f, 0.62f, 0.92f, 0.58f)},
        {1, D2D1::ColorF(0.05f, 0.32f, 0.62f, 0.62f)}};
    ComPtr<ID2D1GradientStopCollection> coll;
    if (FAILED(dc_->CreateGradientStopCollection(stops, 3, &coll))) return false;
    if (FAILED(dc_->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties({0, 0}, {0, 1}), coll.Get(), &fill_))) return false;
    return SUCCEEDED(factory_->CreateStrokeStyle(D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND,
        D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND), nullptr, 0, &round_));
}

static void RainLine(ID2D1DeviceContext* dc, ID2D1Brush* brush, ID2D1StrokeStyle* style,
                     D2D1_POINT_2F a, D2D1_POINT_2F b, float width, const World& w, bool behind) {
    if (!behind) { dc->DrawLine(a, b, brush, width, style); return; }
    auto point = [&](float t) { return D2D1::Point2F(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t); };
    for (auto span : VisibleLine(a.x, a.y, b.x, b.y, width * 0.5f, w))
        dc->DrawLine(point(span.first), point(span.second), brush, width, style);
}

void Renderer::DrawPiles(const Rain& snow) {
    if (Cfg().waterOpacity <= 0) return;
    const float opacity = Cfg().waterOpacity / 100.0f;
    fill_->SetOpacity(opacity);
    outline_->SetOpacity(opacity);
    std::vector<float> top, bot;
    std::vector<D2D1_POINT_2F> pts;
    std::vector<D2D1_QUADRATIC_BEZIER_SEGMENT> segs;
    // Back to front: snow on a nearer window covers snow on a farther one
    std::vector<const Surface*> order;
    for (const Surface& s : snow.surfaces) order.push_back(&s);
    std::stable_sort(order.begin(), order.end(), [](const Surface* a, const Surface* b) { return a->z > b->z; });

    for (const Surface* sp : order) {
        const Surface& s = *sp;
        const int n = s.cells;
        // Visible band: the top is the slightly blurred height, the bottom is the edge of a window in front
        top.resize(n);
        bot.resize(n);
        for (int c = 0; c < n; ++c) {
            float l = c > 0 ? s.h[c - 1] : s.h[c];
            float r = c + 1 < n ? s.h[c + 1] : s.h[c];
            float h = (l + 2 * s.h[c] + r) * 0.25f;
            float wave = (std::sin(c * kCell / (30.0f * s.scale) + snow.Time() * 2.8f + s.left * 0.01f) * 0.65f +
                          std::sin(c * kCell / (13.0f * s.scale) - snow.Time() * 4.2f) * 0.35f);
            wave *= Cfg().waves * 0.035f * s.scale * std::min(1.0f, h / (4 * s.scale));
            top[c] = std::min(std::max(0.0f, h + wave), s.visCeil[c]);
            bot[c] = s.hideBelow[c];
        }
        auto visible = [&](int c) { return top[c] - bot[c] >= 0.3f; };
        auto bottomY = [&](int c) { return bot[c] > 0 ? s.top - bot[c] : s.top + 1.0f; };

        ComPtr<ID2D1PathGeometry> fillGeom, edgeGeom;
        if (FAILED(factory_->CreatePathGeometry(&fillGeom)) || FAILED(factory_->CreatePathGeometry(&edgeGeom))) return;
        ComPtr<ID2D1GeometrySink> fs, es;
        fillGeom->Open(&fs);
        edgeGeom->Open(&es);
        bool any = false;
        for (int c = 0; c < n;) {
            if (!visible(c)) { ++c; continue; }
            int start = c;
            while (c < n && visible(c)) ++c;
            int end = c;
            // Control points every other cell, quadratic curves through the midpoints
            pts.clear();
            for (int i = start; i < end; i += 2) pts.push_back({s.left + (i + 0.5f) * kCell, s.top - top[i]});
            if ((end - 1 - start) % 2) pts.push_back({s.left + (end - 0.5f) * kCell, s.top - top[end - 1]});
            segs.clear();
            for (size_t i = 1; i + 1 < pts.size(); ++i)
                segs.push_back({pts[i], {(pts[i].x + pts[i + 1].x) * 0.5f, (pts[i].y + pts[i + 1].y) * 0.5f}});
            const float x0 = (float)(s.left + start * kCell), x1 = (float)(s.left + end * kCell);
            auto outline = [&](ID2D1GeometrySink* k) {  // left side, top of the drift, right side
                k->AddLine({x0, pts.front().y});
                k->AddLine(pts.front());
                if (!segs.empty()) k->AddQuadraticBeziers(segs.data(), (UINT32)segs.size());
                k->AddLine(pts.back());
                k->AddLine({x1, pts.back().y});
                k->AddLine({x1, bottomY(end - 1)});
            };
            fs->BeginFigure({x0, bottomY(start)}, D2D1_FIGURE_BEGIN_FILLED);
            outline(fs.Get());
            for (int i = end - 2; i >= start; --i)  // back along the bottom edge, with steps
                if (bot[i] != bot[i + 1]) {
                    float xb = (float)(s.left + (i + 1) * kCell);
                    fs->AddLine({xb, bottomY(i + 1)});
                    fs->AddLine({xb, bottomY(i)});
                }
            fs->EndFigure(D2D1_FIGURE_END_CLOSED);
            es->BeginFigure({x0, bottomY(start)}, D2D1_FIGURE_BEGIN_HOLLOW);
            outline(es.Get());
            es->EndFigure(D2D1_FIGURE_END_OPEN);
            any = true;
        }
        fs->Close();
        es->Close();
        if (!any) continue;
        fill_->SetStartPoint({0, s.top - std::max(8.0f, s.maxH)});
        fill_->SetEndPoint({0, (float)s.top});
        dc_->DrawGeometry(edgeGeom.Get(), outline_.Get(), 1.8f * s.scale);  // outline only on top and the sides
        dc_->FillGeometry(fillGeom.Get(), fill_.Get());
        brush_->SetColor(D2D1::ColorF(0.75f, 0.94f, 1.0f, opacity * 0.65f));
        dc_->DrawGeometry(edgeGeom.Get(), brush_.Get(), 0.7f * s.scale);
    }
}

void Renderer::DrawDrops(const Rain& rain, const World& world, bool behind) {
    float opacity = Cfg().rainOpacity / 100.0f;
    if (opacity <= 0) return;
    for (const Drop& f : rain.drops) {
        if (!Cfg().showFalling && (f.kind == DROP_STREAK || f.kind == DROP_LARGE)) continue;
        float v = std::max(1.0f, std::sqrt(f.vx * f.vx + f.vy * f.vy));
        float length = (5.0f + Cfg().streakLength * 0.42f) * f.s * (0.4f + f.z * 0.6f);
        if (f.kind == DROP_SPLASH || f.kind == DROP_RUNOFF) length = std::min(length, f.size * 2.5f);
        D2D1_POINT_2F a = {f.x - f.vx / v * length, f.y - f.vy / v * length}, b = {f.x, f.y};
        float width = std::max(0.65f * f.s, f.size * 0.6f);
        float alpha = f.alpha * opacity;
        if (f.kind == DROP_SPLASH) alpha *= Clampf((f.life - f.age) * 2, 0, 1);
        brush_->SetColor(D2D1::ColorF(0.08f, 0.28f, 0.44f, alpha * 0.30f));
        RainLine(dc_.Get(), brush_.Get(), round_.Get(), a, b, width + 1.2f * f.s, world, behind);
        brush_->SetColor(D2D1::ColorF(0.63f, 0.84f, 1, alpha));
        RainLine(dc_.Get(), brush_.Get(), round_.Get(), a, b, width, world, behind);
        if (f.kind == DROP_LARGE) {
            brush_->SetColor(D2D1::ColorF(0.87f, 0.96f, 1, alpha * 0.9f));
            RainLine(dc_.Get(), brush_.Get(), round_.Get(),
                {f.x - f.vx / v * f.size, f.y - f.vy / v * f.size}, b, width * 1.2f, world, behind);
        }
    }
}

void Renderer::DrawSplashes(const Rain& rain, const World& world, bool behind) {
    if (Cfg().splashes <= 0 || Cfg().rainOpacity <= 0) return;
    for (const auto& f : rain.splashes) {
        bool visible = false;
        for (const auto& s : rain.surfaces) if (s.key == f.surface && s.cells > 0) {
            int cell = std::clamp((int)((f.x - s.left) / kCell), 0, s.cells - 1);
            visible = s.h[cell] + 0.5f >= s.hideBelow[cell];
            break;
        }
        if (!visible) continue;
        float t = f.age / f.life, radius = f.size * (0.2f + t);
        brush_->SetColor(D2D1::ColorF(0.65f, 0.87f, 1, (1 - t) * Cfg().rainOpacity * 0.008f));
        // Ellipse segments are clipped too, so desktop-only rain cannot spill over an app.
        for (int i = 0; i < 12; ++i) {
            float a = i * 6.2831853f / 12, b = (i + 1) * 6.2831853f / 12;
            RainLine(dc_.Get(), brush_.Get(), round_.Get(),
                {f.x + std::cos(a) * radius, f.y + std::sin(a) * radius * 0.20f},
                {f.x + std::cos(b) * radius, f.y + std::sin(b) * radius * 0.20f},
                0.8f * f.scale, world, behind);
        }
        for (int dir : {-1, 1}) {
            float x = f.x + dir * radius * 0.55f;
            float y = f.y - std::sin(t * 3.14159265f) * f.size * 0.65f;
            RainLine(dc_.Get(), brush_.Get(), round_.Get(), {x, y}, {x + dir * f.scale, y + 2 * f.scale},
                     f.scale, world, behind);
        }
    }
}

void Renderer::DrawLightning(const Rain& rain, const World& world, bool behind) {
    float a = rain.BoltOpacity() * Cfg().lightningBrightness / 100.0f;
    if (a < 0.001f) return;
    const auto& r = rain.bolt.monitor;
    dc_->PushAxisAlignedClip(D2D1::RectF((float)r.l, (float)r.t, (float)r.r, (float)r.b), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    if (Cfg().screenFlash) {
        brush_->SetColor(D2D1::ColorF(0.75f, 0.87f, 1, a * 0.14f));
        auto rects = behind ? VisibleDesktopRects(r, world) : std::vector<RectI>{r};
        for (auto rc : rects) dc_->FillRectangle(D2D1::RectF((float)rc.l, (float)rc.t, (float)rc.r, (float)rc.b), brush_.Get());
    }
    for (int pass = 0; pass < 3; ++pass) {
        float mul = pass == 0 ? 7.0f : (pass == 1 ? 3.0f : 1.0f);
        float opacity = a * (pass == 0 ? 0.10f : (pass == 1 ? 0.34f : 1.0f));
        brush_->SetColor(pass < 2 ? D2D1::ColorF(0.38f, 0.62f, 1, opacity) : D2D1::ColorF(0.88f, 0.95f, 1, opacity));
        for (const auto& s : rain.bolt.segments)
            RainLine(dc_.Get(), brush_.Get(), round_.Get(), {s.x1, s.y1}, {s.x2, s.y2}, s.width * mul, world, behind);
    }
    dc_->PopAxisAlignedClip();
}

bool Renderer::Draw(const Rain& rain, const World& world) {
    if (!Ready()) return false;
    dc_->BeginDraw();
    dc_->SetTransform(D2D1::Matrix3x2F::Identity());
    dc_->Clear(D2D1::ColorF(0, 0, 0, 0));
    bool behind = Cfg().mode == MODE_BEHIND;
    DrawLightning(rain, world, behind);
    DrawPiles(rain);
    DrawDrops(rain, world, behind);
    DrawSplashes(rain, world, behind);
    if (FAILED(dc_->EndDraw())) return false;
    HRESULT hr = swap_->Present(1, 0);
    return !(hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET);
}
} // namespace rainfx
