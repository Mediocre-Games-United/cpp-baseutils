#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <stdexcept>
#include <string>

namespace baseutils {
    inline void log_input(std::string txt) {
        printf("\033[0m%s: ",txt.c_str());
    }
    inline void log_error(bool will_throw,std::string txt) {
        printf("\033[31m\033[1mERROR: %s\033[0m\033[0m\n",txt.c_str());
        if (!will_throw) return;

        throw std::runtime_error(txt);
    }
    inline void log_warn(std::string txt) {
        printf("\033[33m\033[1mWARN: %s\033[0m\033[0m\n",txt.c_str());
    }
    inline void log_info(std::string txt) {
        printf("\033[0mINFO: %s\n",txt.c_str());
    }
    inline void log_network(std::string txt) {
        printf("\033[35m\033[1mNETWORK: %s\033[0m\033[0m\n",txt.c_str());
    }
    inline void log_success(std::string txt) {
        printf("\033[32m\033[1mOK: %s\033[0m\033[0m\n",txt.c_str());
    }
    inline void log_debug(std::string txt) {
    #ifndef RELEASE
        printf("\033[36mDEBUG: %s\033[0m\n",txt.c_str());
    #endif
    }
    inline void log_verbose(std::string txt) {
    #ifdef VERBOSE
        printf("\033[34mVERBOSE: %s\033[0m\n",txt.c_str());
    #endif
    }
}

#endif
