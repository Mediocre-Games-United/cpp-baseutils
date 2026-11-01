#pragma once

#include "base_types.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include "vec_types.hpp"

namespace cbu {
    template<typename X>
    inline X min(X f1,X f2) {
        if (f1 < f2) return f1;
        return f2;
    }
    template<typename X>
    inline X max(X f1,X f2) {
        if (f1 > f2) return f1;
        return f2;
    }
    template<>
    inline vec min<vec>(vec v1,vec v2) {
        return vec(min(v1.x,v2.x),min(v1.y,v2.y));
    }
    template<>
    inline vec max<vec>(vec v1,vec v2) {
        return vec(max(v1.x,v2.x),max(v1.y,v2.y));
    }

    template<typename X>
    inline X clamp(X f,X min,X max) {
        if (f > max) return max;
        if (f < min) return min;

        return f;
    }
    template<>
    inline vec clamp<vec>(vec f,vec min,vec max) {
        return vec(clamp(f.x,min.x,max.x),clamp(f.y,min.y,max.y));
    }
    template<typename X>
    inline X round(X f,X s = 1.0) {
        return std::round(f / s) * s;
    }
    template<>
    inline vec round<vec>(vec v1,vec v2) {
        return vec(round(v1.x,v2.x),round(v1.y,v2.y));
    }


    template<typename X,typename T = S64>
    inline T sign(X d) {
        if (d > 0) return 1;
        else if (d < 0) return -1;
        return 0;
    }
    template<typename X>
    inline bool same_sign(X x,X y) {
        return sign(x) == sign(y);
    }
    template<typename X>
    inline X lerp(X &src,X &tgt,F64 alpha) {
        return src + (tgt - src) * alpha;
    }
}
