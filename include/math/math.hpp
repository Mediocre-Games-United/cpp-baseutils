#pragma once

#include "base_types.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include "vec_types.hpp"

namespace cbu {
    template<typename X>
    inline X min(const X &f1,const X &f2) {
        if (f1 < f2) return f1;
        return f2;
    }
    template<typename X>
    inline X max(const X &f1,const X &f2) {
        if (f1 > f2) return f1;
        return f2;
    }
    template<>
    inline vec min<vec>(const vec &v1,const vec &v2) {
        return vec(min(v1.x,v2.x),min(v1.y,v2.y));
    }
    template<>
    inline vec max<vec>(const vec &v1,const vec &v2) {
        return vec(max(v1.x,v2.x),max(v1.y,v2.y));
    }

    template<typename X>
    inline X clamp(const X &f,const X &min,const X &max) {
        if (f > max) return max;
        if (f < min) return min;

        return f;
    }
    template<>
    inline vec clamp<vec>(const vec &f,const vec &min,const vec &max) {
        return vec(clamp(f.x,min.x,max.x),clamp(f.y,min.y,max.y));
    }
    template<typename X>
    inline X round(const X &f,const X &s = 1.0) {
        return std::round(F64(f) / F64(s)) * F64(s);
    }
    template<>
    inline vec round<vec>(const vec &v1,const vec &v2) {
        return vec(round(v1.x,v2.x),round(v1.y,v2.y));
    }


    template<typename X,typename T = S64>
    inline T sign(const X &d) {
        if (d > 0) return 1;
        else if (d < 0) return -1;
        return 0;
    }
    template<typename X>
    inline bool same_sign(const X &x,const X &y) {
        return sign(x) == sign(y);
    }
    template<typename X>
    inline X lerp(const X &src,const X &tgt,F64 alpha) {
        return src + (tgt - src) * alpha;
    }
}
