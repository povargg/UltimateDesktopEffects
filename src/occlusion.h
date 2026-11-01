#pragma once
#include "world.h"

// Parametric intervals visible on the desktop. Shared with headless tests.
inline std::vector<std::pair<float, float>> VisibleLine(float ax, float ay, float bx, float by,
                                                       float padding, const World& world) {
    std::vector<std::pair<float, float>> covered;
    for (const auto& wi : world.wins) {
        float lo=0, hi=1;
        auto slab = [&](float start, float delta, float mn, float mx) {
            if (std::fabs(delta)<0.00001f) return start>=mn && start<=mx;
            float a=(mn-start)/delta, b=(mx-start)/delta;
            if(a>b)std::swap(a,b);
            lo=std::max(lo,a);hi=std::min(hi,b);return lo<=hi;
        };
        if(slab(ax,bx-ax,wi.rc.l-padding,wi.rc.r+padding) &&
           slab(ay,by-ay,wi.rc.t-padding,wi.rc.b+padding)) covered.push_back({lo,hi});
    }
    std::sort(covered.begin(),covered.end());
    std::vector<std::pair<float,float>> visible;
    float start=0;
    for(auto c:covered) {
        if(c.first>start)visible.push_back({start,c.first});
        start=std::max(start,c.second);if(start>=1)return visible;
    }
    if(start<1)visible.push_back({start,1});
    return visible;
}

inline std::vector<RectI> VisibleDesktopRects(RectI monitor, const World& world) {
    std::vector<RectI> rects{monitor};
    for (const auto& wi : world.wins) {
        std::vector<RectI> next;
        for (auto r : rects) {
            RectI i{std::max(r.l,wi.rc.l),std::max(r.t,wi.rc.t),std::min(r.r,wi.rc.r),std::min(r.b,wi.rc.b)};
            if(i.w()<=0 || i.h()<=0) {next.push_back(r);continue;}
            if(r.t<i.t)next.push_back({r.l,r.t,r.r,i.t});
            if(i.b<r.b)next.push_back({r.l,i.b,r.r,r.b});
            if(r.l<i.l)next.push_back({r.l,i.t,i.l,i.b});
            if(i.r<r.r)next.push_back({i.r,i.t,r.r,i.b});
        }
        rects.swap(next);
    }
    return rects;
}
