#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "base_types.hpp"
#include <stdexcept>
#include <iostream>

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
    inline void log_error(bool will_throw,const char * txt) {
        log_error(will_throw,reinterpret_cast<const letter*>(txt));
    }
    inline void log_warn(const char * txt) {
        log_warn(reinterpret_cast<const letter*>(txt));
    }
    inline void log_info(const char * txt) {
        log_info(reinterpret_cast<const letter*>(txt));
    }
    inline void log_network(const char * txt) {
        log_network(reinterpret_cast<const letter*>(txt));
    }
    inline void log_success(const char * txt) {
        log_success(reinterpret_cast<const letter*>(txt));
    }
    inline void log_debug(const char * txt) {
        log_debug(reinterpret_cast<const letter*>(txt));
    }
    inline void log_verbose(const char * txt) {
        log_verbose(reinterpret_cast<const letter*>(txt));
    }
    inline void log_error(bool will_throw,std::string txt) {
        log_error(will_throw,reinterpret_cast<const letter*>(txt.c_str()));
    }
    inline void log_warn(std::string txt) {
        log_warn(reinterpret_cast<const letter*>(txt.c_str()));
    }
    inline void log_info(std::string txt) {
        log_info(reinterpret_cast<const letter*>(txt.c_str()));
    }
    inline void log_network(std::string txt) {
        log_network(reinterpret_cast<const letter*>(txt.c_str()));
    }
    inline void log_success(std::string txt) {
        log_success(reinterpret_cast<const letter*>(txt.c_str()));
    }
    inline void log_debug(std::string txt) {
        log_debug(reinterpret_cast<const letter*>(txt.c_str()));
    }
    inline void log_verbose(std::string txt) {
        log_verbose(reinterpret_cast<const letter*>(txt.c_str()));
    }

    inline void cli_input(string txt) {
        printf("\033[0m%s: ",reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void cli_input(const char *txt) {
        cli_input(reinterpret_cast<const letter*>(txt));
    }
    inline void cli_input(std::string txt) {
        cli_input(reinterpret_cast<const letter*>(txt.c_str()));
    }
    inline void cli_output(string txt) {
        printf("\033[0m%s\n",reinterpret_cast<const char*>(txt.c_str()));
    }
    inline void cli_output(const char *txt) {
        cli_output(reinterpret_cast<const letter*>(txt));
    }
    inline void cli_output(std::string txt) {
        cli_output(reinterpret_cast<const letter*>(txt.c_str()));
    }

    inline string cli_get_string() {
        std::string str{};
        std::cin >> str;

        return reinterpret_cast<const letter*>(str.c_str());
    }
}

#endif
