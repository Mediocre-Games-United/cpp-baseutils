#include "logger.hpp"
#include "ansi.hpp"
#include "cbu_state.hpp"

#ifdef CBU_LOGGER_ENABLE_MUTEX
#define LOGGER_GET_MUTEX std::lock_guard<std::mutex> lock(global_state.logger_mutex)
#endif
#define CBU_LOGGER_ENABLED if (!global_state.logger_enabled) return
#define CBU_LOGGER_IMPORTANT if (global_state.logger_only_important) return

#ifndef LOGGER_GET_MUTEX
#define LOGGER_GET_MUTEX //
#endif
#ifndef CBU_LOGGER_ENABLED
#define CBU_LOGGER_ENABLED //
#endif
#ifndef CBU_LOGGER_IMPORTANT
#define CBU_LOGGER_IMPORTANT //
#endif


namespace cbu::detail {
    inline void print_log(const char* color, const char* label, const std::string& txt) {
        if (cbu::detail::colors_enabled() && color[0] != '\0') {
            std::printf("%s%s: %s\033[0m\n", color, label, txt.c_str());
        } else {
            std::printf("%s: %s\n", label, txt.c_str());
        }
    }
}  // namespace detail

void cbu::log_error(bool will_throw, std::string txt) {
    {
        LOGGER_GET_MUTEX;
        detail::print_log("\033[31;1m", "ERROR", txt);
    }
    if (will_throw) {
        throw std::runtime_error(txt.c_str());
    }
}

void cbu::log_warn(std::string txt) {
    CBU_LOGGER_ENABLED;
    LOGGER_GET_MUTEX;
    detail::print_log("\033[33;1m", "WARN", txt);
}

void cbu::log_info(std::string txt) {
    CBU_LOGGER_ENABLED;
    CBU_LOGGER_IMPORTANT;

    LOGGER_GET_MUTEX;
    detail::print_log("", "INFO", txt);
}

void cbu::log_network(std::string txt) {
    CBU_LOGGER_ENABLED;
    CBU_LOGGER_IMPORTANT;

    LOGGER_GET_MUTEX;
    detail::print_log("\033[35;1m", "NETWORK", txt);
}

void cbu::log_success(std::string txt) {
    CBU_LOGGER_ENABLED;
    LOGGER_GET_MUTEX;
    detail::print_log("\033[32;1m", "OK", txt);
}

void cbu::log_debug(std::string txt) {
    if (!global_state.debug) return;
    CBU_LOGGER_ENABLED;
    CBU_LOGGER_IMPORTANT;

    LOGGER_GET_MUTEX;
    detail::print_log("\033[36m", "DEBUG", txt);
}

void cbu::log_verbose(std::string txt) {
    if (!global_state.verbose) return;
    CBU_LOGGER_ENABLED;
    CBU_LOGGER_IMPORTANT;

    LOGGER_GET_MUTEX;
    detail::print_log("\033[34m", "VERBOSE", txt);
}
