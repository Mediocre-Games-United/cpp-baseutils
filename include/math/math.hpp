#pragma once

#include "base_types.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>

namespace cbu {
    inline int i_min(int i1,int i2) {
        if (i1 < i2) return i1;
        return i2;
    }
    inline int i_max(int i1,int i2) {
        if (i1 > i2) return i1;
        return i2;
    }
    inline int i_clamp(int i,int min,int max) {
        if (i > max) return max;
        if (i < min) return min;

        return i;
    }

    inline F64 f_min(F64 f1,F64 f2) {
        if (f1 < f2) return f1;
        return f2;
    }
    inline F64 f_max(F64 f1,F64 f2) {
        if (f1 > f2) return f1;
        return f2;
    }
    inline F64 f_clamp(F64 f,F64 min,F64 max) {
        if (f > max) return max;
        if (f < min) return min;

        return f;
    }
    inline F64 f_round(F64 f,F64 s = 1.0) {
        return std::round(f / s) * s;
    }

    inline vec v_min(vec v1,vec v2) {
        return vec(f_min(v1.x,v2.x),f_min(v1.y,v2.y));
    }
    inline vec v_max(vec v1,vec v2) {
        return vec(f_max(v1.x,v2.x),f_max(v1.y,v2.y));
    }
    inline vec v_clamp(vec v,vec min,vec max) {
        return vec(f_clamp(v.x,min.x,max.x),f_clamp(v.y,min.y,max.y));
    }
    inline vec v_round(vec v,vec s = vec::one()) {
        return vec(f_round(v.x,s.x),f_round(v.y,s.y));
    }


    inline int f_sign(F64 d) {
        if (d > 0) return 1;
        else if (d < 0) return -1;
        return 0;
    }
    inline int i_sign(int i) {
        if (i > 0) return 1;
        else if (i < 0) return -1;
        return 0;
    }
    inline bool same_sign(F64 x,F64 y) {
        return f_sign(x) == f_sign(y);
    }

    template<typename T>
    inline T lerp(T &src,T &tgt,F64 alpha) {
        return src + (tgt - src) * alpha;
    }
}
