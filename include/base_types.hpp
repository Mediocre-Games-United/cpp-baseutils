#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <format>
#include <string>
#include <unordered_map>
#include <vector>

using string = std::string;
using letter = char32_t;
using fpath = std::filesystem::path;
template <typename T>
using vector = std::vector<T>;
template <typename K,typename T>
using umap = std::unordered_map<K,T>;

using U64 = uint64_t;
using S64 = int64_t;
using U32 = uint32_t;
using S32 = int32_t;
using U16 = uint16_t;
using S16 = int16_t;
using U8 = uint8_t;
using S8 = int8_t;

using F32 = float;
using F64 = double;

# define PI          3.141592653589793238462643383279502884L /* pi */

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


typedef std::chrono::steady_clock Clock;
typedef Clock::time_point TimePoint;

class vec {
public:
    vec(): x(0), y(0) {}
    vec(F64 ix,F64 iy): x(ix), y(iy) {}

    F64 x;
    F64 y;

    inline vec operator-() {
        return vec(-x,-y);
    }
    inline vec operator+(const vec &other) {
        return vec(x + other.x,y + other.y);
    }
    inline vec operator-(const vec &other) {
        return vec(x - other.x,y - other.y);
    }
    inline vec operator*(const vec &other) {
        return vec(x * other.x,y * other.y);
    }
    inline vec operator/(const vec &other) {
        return vec(x / other.x,y / other.y);
    }
    inline vec operator*(const F64 &other) {
        return vec(x * other,y * other);
    }
    inline vec operator/(const F64 &other) {
        return vec(x / other,y / other);
    }
    inline vec &operator+=(const vec &other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    inline vec &operator-=(const vec &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    inline vec &operator*=(const F64 &n) {
        x *= n;
        y *= n;
        return *this;
    }
    inline vec &operator/=(const F64 &n) {
        x /= n;
        y /= n;
        return *this;
    }

    inline F64 magnitude() {
        return std::sqrt(x * x + y * y);
    }
    inline vec unit() {
        F64 m = magnitude();
        if (m <= 0) return vec::zero();
        return *this / magnitude();
    }
    inline vec abs() {
        return vec(std::abs(x),std::abs(y));
    }

    inline static vec zero() { return vec(0,0); }
    inline static vec one() { return vec(1,1); }
    inline static vec half() { return vec(.5,.5); }
    inline static vec right() { return vec(1,0); }
    inline static vec left() { return vec(-1,0); }
    inline static vec down() { return vec(0,1); }
    inline static vec up() { return vec(0,-1); }

    operator string() const {
        return std::format("V2[{},{}]",x,y);
    }
};
template<>
struct std::formatter<vec> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const vec& v, std::format_context& ctx) const {
        return std::format_to(ctx.out(),"{}",string(v));
    }
};

class rect {
public:
    rect(): x(0), y(0), w(0), h(0) {}
    rect(F64 ix,F64 iy,F64 iw,F64 ih): x(ix), y(iy), w(iw), h(ih) {}

    F64 x;
    F64 y;
    F64 w;
    F64 h;
};

template <typename X,typename Base>
inline bool is_of_type(Base *p) { assert(p); return dynamic_cast<X*>(p); }

#define VEC_ZERO vec(0,0)
