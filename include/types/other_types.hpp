#pragma once

#include "base_string.hpp"

namespace cbu {
    struct too_many_exceptions {
        too_many_exceptions(string txt) : msg(txt) {}

        inline string what() { return msg; }
    private:
        string msg;
    };
}

struct color_t {
    color_t() {}
    color_t(F32 br,F32 a = 1.0) : r(br), g(br), b(br), a(a) {}
    color_t(F32 r,F32 g,F32 b,F32 a = 1.0) : r(r), g(g), b(b), a(a) {}
    F32 r = 1.0;
    F32 g = 1.0;
    F32 b = 1.0;
    F32 a = 1.0;

    inline color_t operator*(const color_t &other) {
        return color_t(r * other.r,g * other.g,b * other.b,a * other.a);
    }
};
