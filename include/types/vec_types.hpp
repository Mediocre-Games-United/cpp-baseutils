#pragma once

#include "base_types.hpp"

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

    operator std::string() const {
        return std::format("V2[{},{}]",x,y);
    }
};
template<>
struct std::formatter<vec> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const vec& v, std::format_context& ctx) const {
        return std::format_to(ctx.out(),"{}",v);
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
