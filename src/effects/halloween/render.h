#pragma once
#include "../../common.h"
#include "scene.h"
#include <d2d1_1.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dxgi1_3.h>

namespace halloweenfx {
class Renderer {
public:
    bool Init(HWND hwnd,int width,int height,float scale);
    void Shutdown();
    bool Ready() const {return dc_ && swap_ && dcomp_ && sprites_[0][0];}
    bool Draw(const Scene& leaves,const World& world);
private:
    struct PileCache {
        uintptr_t key=0;uint64_t revision=0;int type=-1,width=0,height=0;
        ComPtr<ID2D1Bitmap1> bitmap;
    };
    bool CreateSprites();
    bool CachePiles(const Scene& leaves);
    void Sprite(float x,float y,float size,float angle,float squash,int family,int variant,float opacity,bool resting);
    void DrawWebSpiders(const Scene&,const World&);
    void DrawPiles(const Scene& leaves,const World& world);
    void DrawAirborne(const Scene& leaves,const World& world);
    ComPtr<ID3D11Device> d3d_;
    ComPtr<IDXGISwapChain1> swap_;
    ComPtr<ID2D1Factory1> factory_;
    ComPtr<ID2D1Device> d2dDevice_;
    ComPtr<ID2D1DeviceContext> dc_;
    ComPtr<ID2D1Bitmap1> target_;
    ComPtr<IDCompositionDevice> dcomp_;
    ComPtr<IDCompositionTarget> dcompTarget_;
    ComPtr<IDCompositionVisual> visual_;
    ComPtr<ID2D1Bitmap1> sprites_[2][6];
    std::vector<PileCache> piles_;
    ComPtr<ID2D1SolidColorBrush> webBrush_;
    float scale_=1;
};
} // namespace halloweenfx
