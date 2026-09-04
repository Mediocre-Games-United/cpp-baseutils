#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "base_types.hpp"
#include <stdexcept>
#include <iostream>
#include <string>

namespace baseutils {
    inline void log_error(bool will_throw,string txt) {
        printf("\033[31m\033[1mERROR: %s\033[0m\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
        if (!will_throw) return;

        throw std::runtime_error(reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void log_warn(string txt) {
        printf("\033[33m\033[1mWARN: %s\033[0m\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void log_info(string txt) {
        printf("\033[0mINFO: %s\n",reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void log_network(string txt) {
        printf("\033[35m\033[1mNETWORK: %s\033[0m\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void log_success(string txt) {
        printf("\033[32m\033[1mOK: %s\033[0m\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void log_debug(string txt) {
        #ifndef RELEASE
        printf("\033[36mDEBUG: %s\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
        #endif
    }
    inline void log_verbose(string txt) {
        #ifdef VERBOSE
        printf("\033[34mVERBOSE: %s\033[0m\n",reinterpret_cast<const char*>(txt.c_str()));
        #endif
    }

    inline void cli_input(string txt) {
        printf("\033[0m%s: ",reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void cli_output(string txt) {
        printf("\033[0m%s\n",reinterpret_cast<const char*>(txt.c_str()));
    }

    inline string cli_get_string() {
        string str{};
        std::getline(std::cin,str);

        return str;
    }
}

#endif
