#pragma once

#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

using string = std::string;
using letter = char32_t;
using fpath = std::filesystem::path;
template <typename T>
using vector = std::vector<T>;

# define PI          3.141592653589793238462643383279502884L /* pi */

struct color_t {
    color_t() {}
    color_t(float br,float a = 1.0) : r(br), g(br), b(br), a(a) {}
    color_t(float r,float g,float b,float a = 1.0) : r(r), g(g), b(b), a(a) {}
    float r = 1.0;
    float g = 1.0;
    float b = 1.0;
    float a = 1.0;

    inline color_t operator*(const color_t &other) {
        return color_t(r * other.r,g * other.g,b * other.b,a * other.a);
    }
};


typedef std::chrono::steady_clock Clock;
typedef Clock::time_point TimePoint;

class vec {
public:
    vec(): x(0), y(0) {}
    vec(double ix,double iy): x(ix), y(iy) {}

    double x;
    double y;
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
    inline vec operator*(const double &other) {
        return vec(x * other,y * other);
    }
    inline vec operator/(const double &other) {
        return vec(x / other,y / other);
    }
    inline double magnitude() {
        return std::sqrt(x * x + y * y);
    }
    inline vec unit() {
        double m = magnitude();
        if (m <= 0) return vec::zero();
        return *this / magnitude();
    }
    inline vec abs() {
        return vec(std::abs(x),std::abs(y));
    }

    inline static vec zero() { return vec(0,0); }
};
class rect {
public:
    rect(): x(0), y(0), w(0), h(0) {}
    rect(double ix,double iy,double iw,double ih): x(ix), y(iy), w(iw), h(ih) {}

    double x;
    double y;
    double w;
    double h;
};

#define VEC_ZERO vec(0,0)
