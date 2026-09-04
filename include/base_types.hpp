#pragma once

#include <filesystem>
#include <string>
#include <format>

using string = std::u32string;
using letter = char32_t;
using fpath = std::filesystem::path;

template<>
struct std::formatter<string> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const string &obj,std::format_context &ctx) const {
        return std::format_to(ctx.out(),"{}",reinterpret_cast<const char*>(obj.c_str()));
    }
};
