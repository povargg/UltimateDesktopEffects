#include "render.h"
#include "../../occlusion.h"
#include "sprites.h"
#include "sprite_geometry.h"

namespace halloweenfx {

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

    if (!CreateSprites()) { Shutdown(); return false; }
    return true;
}

void Renderer::Shutdown() {
    piles_.clear();webBrush_.Reset();for(auto& group:sprites_)for(auto& sprite:group)sprite.Reset();
    visual_.Reset();dcompTarget_.Reset();dcomp_.Reset();
    if(dc_)dc_->SetTarget(nullptr);
    target_.Reset();dc_.Reset();d2dDevice_.Reset();factory_.Reset();swap_.Reset();d3d_.Reset();
}
bool Renderer::CreateSprites() {
    auto p=D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED));
    for(int type=0;type<2;++type)for(int variant=0;variant<kSpriteVariants[type];++variant)
        if(FAILED(dc_->CreateBitmap(D2D1::SizeU(16,16),kSprites[type][variant],16*4,&p,&sprites_[type][variant])))return false;
    return SUCCEEDED(dc_->CreateSolidColorBrush(D2D1::ColorF(1,1,1,1),&webBrush_));
}
void Renderer::Sprite(float x,float y,float size,float angle,float squash,int family,int variant,float opacity,bool resting) {
    x=std::round(x);y=std::round(y);
    D2D1_POINT_2F center={x,y};
    angle=std::round(angle/22.5f)*22.5f;
    auto rotation=D2D1::Matrix3x2F::Rotation(angle,center);
    auto scaling=D2D1::Matrix3x2F::Scale(1.0f,squash,center);
    dc_->SetTransform(resting?rotation*scaling:scaling*rotation);
    int type=std::clamp(family,0,1);
    dc_->DrawBitmap(sprites_[type][variant%kSpriteVariants[type]].Get(),D2D1::RectF(x-size*0.5f,y-size*0.5f,x+size*0.5f,y+size*0.5f),
        opacity,D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    dc_->SetTransform(D2D1::Matrix3x2F::Identity());
}
bool Renderer::CachePiles(const Scene& leaves) {
    piles_.erase(std::remove_if(piles_.begin(),piles_.end(),[&](const PileCache& p){
        for(const auto& s:leaves.surfaces)if(s.key==p.key && !s.objects.empty())return false;
        return true;
    }),piles_.end());
    for(const auto& s:leaves.surfaces) {
        if(s.objects.empty())continue;
        PileCache* cache=nullptr;
        for(auto& p:piles_)if(p.key==s.key){cache=&p;break;}
        if(!cache){piles_.emplace_back();cache=&piles_.back();cache->key=s.key;}
        int width=std::max(1,s.width),height=std::max(4,(int)std::ceil(s.maxH+56*s.scale));
        if(cache->bitmap && cache->revision==s.revision && cache->type==Cfg().contentMode && cache->width==width && cache->height==height)continue;
        if(!cache->bitmap || cache->width!=width || cache->height!=height) {
            cache->bitmap.Reset();
            auto props=D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED));
            if(FAILED(dc_->CreateBitmap(D2D1::SizeU(width,height),nullptr,0,&props,&cache->bitmap)))return false;
            cache->width=width;cache->height=height;
        }
        dc_->SetTarget(cache->bitmap.Get());dc_->BeginDraw();
        dc_->SetTransform(D2D1::Matrix3x2F::Identity());dc_->Clear(D2D1::ColorF(0,0,0,0));
        dc_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
        for(const auto& l:s.objects) {
            Sprite(l.x,cache->height-RestingParticleCenterHeight(l,l.family),l.size,l.rotation,
                   kRestingSpriteSquash,l.family,l.variant,1,true);
        }
        HRESULT hr=dc_->EndDraw();dc_->SetTarget(target_.Get());
        if(FAILED(hr))return false;
        cache->revision=s.revision;cache->type=Cfg().contentMode;
    }
    return true;
}
void Renderer::DrawPiles(const Scene& leaves,const World& world) {
    if(Cfg().pileOpacity<=0)return;
    std::vector<const Surface*> order;
    for(const auto& s:leaves.surfaces)if(!s.objects.empty())order.push_back(&s);
    std::stable_sort(order.begin(),order.end(),[](const Surface* a,const Surface* b){return a->z>b->z;});
    for(auto ptr:order) {
        const auto& s=*ptr;const PileCache* cache=nullptr;
        for(const auto& p:piles_)if(p.key==s.key){cache=&p;break;}
        if(!cache || !cache->bitmap)continue;
        World foreground;
        foreground.wins.assign(world.wins.begin(),world.wins.begin()+std::min((size_t)std::max(0,s.z),world.wins.size()));
        RectI bounds{s.left,s.top-cache->height,s.left+cache->width,s.top};
        auto visible=VisibleDesktopRects(bounds,foreground);
        for(const auto& r:visible) {
            dc_->PushAxisAlignedClip(D2D1::RectF((float)r.l,(float)r.t,(float)r.r,(float)r.b),D2D1_ANTIALIAS_MODE_ALIASED);
            dc_->DrawBitmap(cache->bitmap.Get(),D2D1::RectF((float)bounds.l,(float)bounds.t,(float)bounds.r,(float)bounds.b),
                Cfg().pileOpacity/100.0f,D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
            dc_->PopAxisAlignedClip();
        }
    }
}
void Renderer::DrawAirborne(const Scene& leaves,const World& world) {
    if(Cfg().objectOpacity<=0)return;
    bool behind=Cfg().mode==MODE_BEHIND;
    for(const auto& f:leaves.airborne) {
        if(!Cfg().showFalling && f.kind==PARTICLE_FALLING)continue;
        float a=f.rotation*57.2957795f;
        float squash=1.0f-Cfg().tumble/100.0f*(0.62f-0.62f*std::fabs(std::cos(f.rotation*0.7f+f.phase)));
        float opacity=Cfg().objectOpacity/100.0f*(0.7f+0.3f*f.z);
        if(f.kind==PARTICLE_LIFTED)opacity*=Clampf((f.life-f.age)*1.8f,0,1);
        if(!behind){Sprite(f.x,f.y,f.size,a,squash,f.family,f.variant,opacity,false);continue;}
        int radius=(int)std::ceil(f.size*0.75f);
        RectI bounds{(int)f.x-radius,(int)f.y-radius,(int)f.x+radius,(int)f.y+radius};
        for(const auto& r:VisibleDesktopRects(bounds,world)) {
            dc_->PushAxisAlignedClip(D2D1::RectF((float)r.l,(float)r.t,(float)r.r,(float)r.b),D2D1_ANTIALIAS_MODE_ALIASED);
            Sprite(f.x,f.y,f.size,a,squash,f.family,f.variant,opacity,false);
            dc_->PopAxisAlignedClip();
        }
    }
}
void Renderer::DrawWebSpiders(const Scene& scene,const World& world){
 if(!Cfg().showFalling||Cfg().objectOpacity<=0)return;
 bool behind=Cfg().mode==MODE_BEHIND;
 for(const auto& s:scene.webs){
  dc_->PushAxisAlignedClip(D2D1::RectF((float)s.monitor.l,(float)s.monitor.t,(float)s.monitor.r,(float)s.monitor.b),D2D1_ANTIALIAS_MODE_ALIASED);
  auto thread=Scene::Thread(s);
  if(Cfg().webOpacity>0&&thread.by>thread.ay){
   auto spans=behind?VisibleLine(thread.ax,thread.ay,thread.bx,thread.by,0,world):std::vector<std::pair<float,float>>{{0,1}};
   for(auto span:spans){
    D2D1_POINT_2F a={thread.ax+(thread.bx-thread.ax)*span.first,thread.ay+(thread.by-thread.ay)*span.first};
    D2D1_POINT_2F b={thread.ax+(thread.bx-thread.ax)*span.second,thread.ay+(thread.by-thread.ay)*span.second};
    float opacity=Cfg().webOpacity*.01f*Cfg().objectOpacity*.01f;
    webBrush_->SetColor(D2D1::ColorF(.08f,.05f,.12f,opacity*.5f));dc_->DrawLine(a,b,webBrush_.Get(),2.5f*s.scale);
    webBrush_->SetColor(D2D1::ColorF(.87f,.84f,.95f,opacity));dc_->DrawLine(a,b,webBrush_.Get(),s.scale);
   }
  }
  float angle=s.state==SPIDER_ASCENDING?180.0f:0.0f;
  auto draw=[&]{Sprite(s.x,s.y,s.size,angle,1,FAMILY_SPIDER,s.variant,Cfg().objectOpacity*.01f,false);};
  if(!behind)draw();
  else{
   int radius=(int)std::ceil(s.size*.75f);RectI bounds{(int)s.x-radius,(int)s.y-radius,(int)s.x+radius,(int)s.y+radius};
   for(auto rc:VisibleDesktopRects(bounds,world)){
    dc_->PushAxisAlignedClip(D2D1::RectF((float)rc.l,(float)rc.t,(float)rc.r,(float)rc.b),D2D1_ANTIALIAS_MODE_ALIASED);draw();dc_->PopAxisAlignedClip();
   }
  }
  dc_->PopAxisAlignedClip();
 }
}
bool Renderer::Draw(const Scene& leaves,const World& world) {
    if(!Ready() || !CachePiles(leaves))return false;
    dc_->SetTarget(target_.Get());dc_->BeginDraw();
    dc_->SetTransform(D2D1::Matrix3x2F::Identity());dc_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    dc_->Clear(D2D1::ColorF(0,0,0,0));DrawPiles(leaves,world);DrawAirborne(leaves,world);DrawWebSpiders(leaves,world);
    if(FAILED(dc_->EndDraw()))return false;
    HRESULT hr=swap_->Present(1,0);
    return !(hr==DXGI_ERROR_DEVICE_REMOVED || hr==DXGI_ERROR_DEVICE_RESET);
}
} // namespace halloweenfx
