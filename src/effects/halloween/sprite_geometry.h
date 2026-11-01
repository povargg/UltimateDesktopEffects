#pragma once
#include "scene.h"
#include "sprites.h"
#include <array>

namespace halloweenfx {

struct SpriteBounds {
    float left=1,top=1,right=-1,bottom=-1;
};

// Match the renderer's 22.5-degree rotation steps, including negative angles.
inline int ParticleRotationFrame(float degrees) {
    int frame=(int)std::lround(degrees/22.5f)%16;
    return frame<0?frame+16:frame;
}

inline const SpriteBounds& ParticleOpaqueBounds(int type,int variant,float degrees) {
    static const auto table=[] {
        std::array<SpriteBounds,2*6*16> result{};
        for(int t=0;t<2;++t)for(int v=0;v<kSpriteVariants[t];++v)for(int frame=0;frame<16;++frame) {
            auto& b=result[(t*6+v)*16+frame];
            float angle=frame*3.141592653589793f/8;
            float cosine=std::cos(angle),sine=std::sin(angle);
            for(int y=0;y<16;++y)for(int x=0;x<16;++x) {
                if((kSprites[t][v][y*16+x]>>24)==0)continue;
                for(int cornerY=0;cornerY<2;++cornerY)for(int cornerX=0;cornerX<2;++cornerX) {
                    float px=(x+cornerX)/16.0f-0.5f,py=(y+cornerY)/16.0f-0.5f;
                    float rx=px*cosine-py*sine,ry=px*sine+py*cosine;
                    b.left=std::min(b.left,rx);b.right=std::max(b.right,rx);
                    b.top=std::min(b.top,ry);b.bottom=std::max(b.bottom,ry);
                }
            }
        }
        return result;
    }();
    type=std::clamp(type,0,1);
    return table[(type*6+std::clamp(variant,0,kSpriteVariants[type]-1))*16+ParticleRotationFrame(degrees)];
}

inline float RestingParticleCenterHeight(const SettledObject& leaf,int type) {
    // The lowest opaque pixel, rather than the transparent sprite border, touches the stack.
    return leaf.base+ParticleOpaqueBounds(type,leaf.variant,leaf.rotation).bottom*leaf.size;
}

inline float RestingParticleVisibleTop(const SettledObject& leaf,int type) {
    const auto& b=ParticleOpaqueBounds(type,leaf.variant,leaf.rotation);
    return leaf.base+(b.bottom-b.top)*leaf.size;
}
} // namespace halloweenfx
