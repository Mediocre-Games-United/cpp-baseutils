#pragma once

#include "base_types.hpp"
#include <format>
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace cbu {
    namespace detail {
        inline bool colors_enabled() {
            #ifdef _WIN32
            static const bool enabled = [] {
                if (!_isatty(_fileno(stdout))) return false;

                HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
                if (output == INVALID_HANDLE_VALUE || output == nullptr) return false;

                DWORD mode = 0;
                if (!GetConsoleMode(output, &mode)) return false;

                return SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
            }();
            return enabled;
            #else
            return isatty(fileno(stdout)) != 0;
            #endif
        }
    }

    struct ansi_color_code {
        ansi_color_code(string c) : label(c) {}
        string label;
    };
    const ansi_color_code ansi_color_white = {"0"};

    const ansi_color_code ansi_color_black = {"30"};
    const ansi_color_code ansi_color_dark_red = {"31"};
    const ansi_color_code ansi_color_green = {"32"};
    const ansi_color_code ansi_color_orange = {"33"};
    const ansi_color_code ansi_color_blue = {"34"};
    const ansi_color_code ansi_color_magenta = {"35"};
    const ansi_color_code ansi_color_dark_cyan = {"36"};

    const ansi_color_code ansi_color_gray = {"90"};
    const ansi_color_code ansi_color_red = {"91"};
    const ansi_color_code ansi_color_lime = {"92"};
    const ansi_color_code ansi_color_yellow = {"93"};
    const ansi_color_code ansi_color_light_blue = {"94"};
    const ansi_color_code ansi_color_pink = {"95"};
    const ansi_color_code ansi_color_cyan = {"96"};

    inline string ansi_wrap_string(string txt,ansi_color_code code) {
        return std::format("\033[{}m{}\033[0m",code.label,txt);
    }
    inline string ansi_wrap_bold(string txt) { return std::format("\033[1m{}\033[0m",txt); }
    inline string ansi_wrap_italic(string txt) { return std::format("\033[3m{}\033[0m",txt); }
}
