#pragma once

#include <filesystem>
#include <string>

using string = std::string;
using letter = char32_t;
using fpath = std::filesystem::path;

struct color_t {
    color_t() {}
    color_t(float br,float a = 1.0) : r(br), g(br), b(br), a(a) {}
    color_t(float r,float g,float b,float a = 1.0) : r(r), g(g), b(b), a(a) {}
    float r = 1.0;
    float g = 1.0;
    float b = 1.0;
    float a = 1.0;
};
