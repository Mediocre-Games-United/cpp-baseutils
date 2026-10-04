#pragma once

#include "base_types.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>

namespace cbu {
    inline F64 angle_clamp(F64 a) {
        a = fmod(a + PI, PI * 2.0);
        if (a <= 0) a += PI * 2.0;
        return a - PI;
    }
    inline F64 angle_diff(F64 a1, F64 a2) {
        F64 diff = angle_clamp(a2) - angle_clamp(a1);

        return angle_clamp(diff);
    }
    inline F64 angle_to_deg(F64 a) {
        return a / PI * 180.0;
    }
    inline F64 vec_to_angle(vec v) {
        F64 a = atan2(v.y,v.x);
        if (std::isnan(a)) return 0.0;

        return a;
    }
    inline vec angle_to_vec(F64 a) {
        return vec(cos(a),sin(a));
    }
    inline int angle_to_x(F64 a) {
        a = angle_clamp(a);
        if (std::abs(a) > PI / 2.0) return -1;

        return 1;
    }
    inline F64 angle_x_to_flip(F64 a,int x) {
        a = angle_clamp(a);
        if (x < 0) {
            if (a > 0) return PI + a;
            else return a - PI;
        }

        return a;
    }


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

    inline vec v_min(vec v1,vec v2) {
        return vec(f_min(v1.x,v2.x),f_min(v1.y,v2.y));
    }
    inline vec v_max(vec v1,vec v2) {
        return vec(f_max(v1.x,v2.x),f_max(v1.y,v2.y));
    }
    inline vec v_clamp(vec v,vec min,vec max) {
        return vec(f_clamp(v.x,min.x,max.x),f_clamp(v.y,min.y,max.y));
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
