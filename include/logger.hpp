#pragma once

#include "find_config.h"
#include "base_types.hpp"
#include <stdexcept>

namespace cbu {
#ifdef CBU_LOGGER_ENABLE_MUTEX
    extern std::mutex logger_mutex;
#define LOGGER_GET_MUTEX std::lock_guard<std::mutex> lock(logger_mutex)
#else
#define LOGGER_GET_MUTEX // no logger mutex enabled!
#endif
    inline void log_error(bool will_throw,string txt) {
        LOGGER_GET_MUTEX;
        printf("\033[31m\033[1mERROR: %s\033[0m\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
        if (!will_throw) return;

        throw std::runtime_error(reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void log_warn(string txt) {
        LOGGER_GET_MUTEX;
        printf("\033[33m\033[1mWARN: %s\033[0m\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void log_info(string txt) {
        LOGGER_GET_MUTEX;
        printf("\033[0mINFO: %s\n",reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void log_network(string txt) {
        LOGGER_GET_MUTEX;
        printf("\033[35m\033[1mNETWORK: %s\033[0m\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void log_success(string txt) {
        LOGGER_GET_MUTEX;
        printf("\033[32m\033[1mOK: %s\033[0m\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void log_debug(string txt) {
        #ifndef RELEASE
        LOGGER_GET_MUTEX;
        printf("\033[36mDEBUG: %s\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
        #endif
    }
    inline void log_verbose(string txt) {
        #ifdef VERBOSE
        LOGGER_GET_MUTEX;
        printf("\033[34mVERBOSE: %s\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
        #endif
    }
}
