#pragma once

#include "find_config.hpp"
#include "ansi.hpp"
#include <cstdio>
#include <stdexcept>
#include <mutex>

#ifdef CBU_ENABLE_GLOBAL_STATE
#include "cbu_state.hpp"
#endif


namespace cbu {
#ifdef CBU_ENABLE_GLOBAL_STATE
#ifdef CBU_LOGGER_ENABLE_MUTEX
#define LOGGER_GET_MUTEX std::lock_guard<std::mutex> lock(global_state.logger_mutex)
#endif
#define CBU_LOGGER_ENABLED if (!global_state.logger_enabled) return
#define CBU_LOGGER_IMPORTANT if (global_state.logger_only_important) return
#endif

#ifndef LOGGER_GET_MUTEX
#define LOGGER_GET_MUTEX //
#endif
#ifndef CBU_LOGGER_ENABLED
#define CBU_LOGGER_ENABLED //
#endif
#ifndef CBU_LOGGER_IMPORTANT
#define CBU_LOGGER_IMPORTANT //
#endif

    namespace detail {

        inline void print_log(const char* color, const char* label, const std::string& txt) {
            if (colors_enabled() && color[0] != '\0') {
                std::printf("%s%s: %s\033[0m\n", color, label, txt.c_str());
            } else {
                std::printf("%s: %s\n", label, txt.c_str());
            }
        }
    }  // namespace detail

    inline void log_error(bool will_throw, std::string txt) {
        {
            LOGGER_GET_MUTEX;
            detail::print_log("\033[31;1m", "ERROR", txt);
        }
        if (will_throw) {
            throw std::runtime_error(txt.c_str());
        }
    }

    inline void log_warn(std::string txt) {
        CBU_LOGGER_ENABLED;
        LOGGER_GET_MUTEX;
        detail::print_log("\033[33;1m", "WARN", txt);
    }

    inline void log_info(std::string txt) {
        CBU_LOGGER_ENABLED;
        CBU_LOGGER_IMPORTANT;

        LOGGER_GET_MUTEX;
        detail::print_log("", "INFO", txt);
    }

    inline void log_network(std::string txt) {
        CBU_LOGGER_ENABLED;
        CBU_LOGGER_IMPORTANT;

        LOGGER_GET_MUTEX;
        detail::print_log("\033[35;1m", "NETWORK", txt);
    }

    inline void log_success(std::string txt) {
        CBU_LOGGER_ENABLED;
        LOGGER_GET_MUTEX;
        detail::print_log("\033[32;1m", "OK", txt);
    }

    inline void log_debug(std::string txt) {
        CBU_LOGGER_ENABLED;
        CBU_LOGGER_IMPORTANT;

        #ifndef RELEASE
        LOGGER_GET_MUTEX;
        detail::print_log("\033[36m", "DEBUG", txt);
        #endif
    }

    inline void log_verbose(std::string txt) {
        CBU_LOGGER_ENABLED;
        CBU_LOGGER_IMPORTANT;

#ifndef VERBOSE
        if (!global_state.verbose) return;
#endif
        LOGGER_GET_MUTEX;
        detail::print_log("\033[34m", "VERBOSE", txt);
    }
}  // namespace cbu
