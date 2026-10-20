#pragma once
#include "base_types.hpp"

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
}
