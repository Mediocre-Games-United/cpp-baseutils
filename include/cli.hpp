#pragma once

#include "base_types.hpp"
#include "file.hpp"
#include "logger.hpp"
#include "stringmath.hpp"
#include <filesystem>
#include <format>
#include <iostream>
#include <string>

namespace cbu {
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

    inline bool cli_get_valid_string(string *target) {
        string raw = cbu::cli_get_string();
        if (raw.empty()) return false;

        *target = raw;
        return true;
    }
#ifdef _WIN32
    inline bool cli_get_valid_dirpath(fpath *target,fpath cwd = "C:/") {
#else
        inline bool cli_get_valid_dirpath(fpath *target,fpath cwd = "/") {
#endif
        cli_output("Welcome to the directory select utility!");
        string cmd;
        while (true) {
            cli_input(std::format("@[{}]: q to cancel h to help",path_to_utf8(cwd)));
            cmd = cli_get_string();
            if (cmd == "h") {
                cli_output("[ls] list the current directory\n[cd] change working directory\n[s] submit current directory\n[q] cancel");
                continue;
            } if (cmd == "q") {
                return false;
            } if (cmd == "s") {
                log_debug(std::format("Submitting directory {}",path_to_utf8(cwd)));
                break;
            } if (cmd == "ls") {
                auto ret = iterate_dir(cwd);
                bool is_empty = true;
                for (auto &s : ret) {
                    if (!std::filesystem::is_directory(s)) continue;
                    is_empty = false;
                    auto rel = std::filesystem::relative(s,cwd);
                    cli_output(std::format("{}",path_to_utf8(rel)));
                }
                if (is_empty) cli_output("Current working directory empty");
                continue;
            } if (cmd.starts_with("cd ")) {
                string sub = cmd.substr(3);
                fpath n = cwd / sub;
                n = n.lexically_normal();
                if (!std::filesystem::is_directory(n)) {
                    log_warn(std::format("{} is not a directory!",path_to_utf8(n)));
                    continue;
                }

                cwd = n;
            }
        }

        *target = cwd;
        return true;
    }
    inline bool cli_get_valid_int(int *target,int min,int max) {
        string str = cbu::cli_get_string();
        if (str.empty()) return false;
        int raw = atoi(str.c_str());
        if (raw < min) return false;
        if (raw > max) return false;
        *target = raw;

        return true;
    }
    inline bool cli_get_valid_bool(bool *target) {
        string raw = cbu::cli_get_string();
        if (raw.empty()) return false;
        raw = cbu::string_uppercase(raw);
        if (raw == "TRUE" || raw == "YES" || raw == "Y" || raw == "T") {
            *target = true;
            return true;
        } if (raw == "FALSE" || raw == "NO" || raw == "N" || raw == "F") {
            *target = false;
            return true;
        }

        return false;
    }
}
