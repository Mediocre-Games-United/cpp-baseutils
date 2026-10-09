#pragma once

#include "find_config.h"
#include "base_types.hpp"

#include <cstdio>
#include <stdexcept>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace cbu {
    #ifdef CBU_LOGGER_ENABLE_MUTEX
    extern std::mutex logger_mutex;
    #define LOGGER_GET_MUTEX std::lock_guard<std::mutex> lock(logger_mutex)
    #else
    #define LOGGER_GET_MUTEX // no logger mutex enabled!
    #endif

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

        inline void print_log(const char* color, const char* label, const string& txt) {
            if (colors_enabled() && color[0] != '\0') {
                std::printf("%s%s: %s\033[0m\n", color, label, txt.c_str());
            } else {
                std::printf("%s: %s\n", label, txt.c_str());
            }
        }
    }  // namespace detail

    inline void log_error(bool will_throw, string txt) {
        {
            LOGGER_GET_MUTEX;
            detail::print_log("\033[31;1m", "ERROR", txt);
        }
        if (will_throw) {
            throw std::runtime_error(txt.c_str());
        }
    }

    inline void log_warn(string txt) {
        LOGGER_GET_MUTEX;
        detail::print_log("\033[33;1m", "WARN", txt);
    }

    inline void log_info(string txt) {
        LOGGER_GET_MUTEX;
        detail::print_log("", "INFO", txt);
    }

    inline void log_network(string txt) {
        LOGGER_GET_MUTEX;
        detail::print_log("\033[35;1m", "NETWORK", txt);
    }

    inline void log_success(string txt) {
        LOGGER_GET_MUTEX;
        detail::print_log("\033[32;1m", "OK", txt);
    }

    inline void log_debug(string txt) {
        #ifndef RELEASE
        LOGGER_GET_MUTEX;
        detail::print_log("\033[36m", "DEBUG", txt);
        #endif
    }

    inline void log_verbose(string txt) {
        #ifdef VERBOSE
        LOGGER_GET_MUTEX;
        detail::print_log("\033[34m", "VERBOSE", txt);
        #endif
    }
}  // namespace cbu
