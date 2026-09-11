#pragma once

#include "base_types.hpp"
#include <cmath>
#include <cstdlib>

namespace cbu {
    inline double angle_clamp(double a) {
        a = fmod(a + PI, PI * 2.0);
        if (a <= 0) a += PI * 2.0;
        return a - PI;
    }
    inline double angle_diff(double a1, double a2) {
        double diff = angle_clamp(a2) - angle_clamp(a1);

        return angle_clamp(diff);
    }
    inline double angle_to_deg(double a) {
        return a / PI * 180.0;
    }
    inline double vec_to_angle(vec v) {
        double a = atan2(v.y,v.x);
        if (std::isnan(a)) return 0.0;

        return a;
    }
    inline vec angle_to_vec(double a) {
        return vec(cos(a),sin(a));
    }
    inline int angle_to_x(double a) {
        a = angle_clamp(a);
        if (std::abs(a) > PI / 2.0) return -1;

        return 1;
    }
    inline double angle_x_to_flip(double a,int x) {
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

    inline double f_min(double f1,double f2) {
        if (f1 < f2) return f1;
        return f2;
    }
    inline double f_max(double f1,double f2) {
        if (f1 > f2) return f1;
        return f2;
    }
    inline double f_clamp(double f,double min,double max) {
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


    inline int f_sign(double d) {
        if (d > 0) return 1;
        else if (d < 0) return -1;
        return 0;
    }
    inline int i_sign(int i) {
        if (i > 0) return 1;
        else if (i < 0) return -1;
        return 0;
    }
}
