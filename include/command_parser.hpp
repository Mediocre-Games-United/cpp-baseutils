#pragma once

#include "base_types.hpp"
#include "cli.hpp"
#include "logger.hpp"
#include <format>
#include <unordered_set>
#include "find_config.h"

namespace cbu {
    struct parser_flag {
        parser_flag(char sk,string lk = "",string help = "") : long_k(lk), short_k(sk), help(help) {}

        string long_k;
        char short_k;
        string help;

        bool operator ==(parser_flag &other) {
            return short_k == other.short_k || (!long_k.empty() and long_k == other.long_k);
        }
    };
    struct parser_val {
        parser_val(parser_flag flag,size_t argc = 1) : argc(argc), flag(flag) {}

        size_t argc;
        parser_flag flag;
    };
    struct parser_output {
        std::unordered_set<char> flags{};
        umap<string,string> values{};
        string cmd = "";
        string err = "";
        bool valid = false;
        bool can_proceed = false;

        operator string() const {
            string fl = "";
            for (auto &s : flags) {
                fl = std::format("{}, {}",fl,s);
            }
            if (flags.empty()) fl = "<none>";
            else fl = fl.substr(2);
            string val = "";
            for (auto &s : values) {
                val = std::format("{}, {}={}",val,s.first,s.second);
            }
            if (values.empty()) val = "<none>";
            else val = val.substr(2);

            return std::format("flags: {}, values: {}, cmd: {}",fl,val,cmd);
        }
    };

    inline string format_flag_name(parser_flag &fl) {
        string full = "";
        full += std::format("[-{}] ",fl.short_k);
        if (!fl.long_k.empty()) full += std::format("[--{}] ",fl.long_k);

        return full;
    }
    inline void set_parser_help(parser_output &pt,vector<parser_flag> flags,vector<parser_val> vals) {
        pt.valid = true;
        pt.can_proceed = false;
        string full = "[--version] Show the version information\n[-h] [--help] Show this list\n";
        for (auto &s : flags) {
            full += format_flag_name(s);

            if (s.help.empty()) full += "\n";
            else full += std::format("{}\n",s.help);
        }
        for (auto &s : vals) {
            s.
        }

        cbu::cli_output(std::format("---\n{}---",full));
    }
    inline void set_parser_version(parser_output &pt) {
        pt.valid = true;
        pt.can_proceed = false;

        cbu::cli_output(std::format("---\nVersion: {}\n---",VERSION_STRING));
    }
    inline parser_output parse_args(int argc,char **argv,vector<parser_flag> flags,vector<parser_val> vals) {
        parser_output output{};

        // size_t val_count = 0;
        for (int i = 1; i < argc; i ++) {
            string cur = argv[i];

            cbu::log_debug(cur);

            if (cur[0] != '-') {
                if (output.cmd.empty()) output.cmd = cur;
                else output.cmd = std::format("{} {}",output.cmd,cur);

                continue;
            }

            if (cur[1] == '-') {
                string lg = cur.substr(2);

                if (lg == "help") {
                    set_parser_help(output,flags,vals);
                    return output;
                } if (lg == "version") {
                    set_parser_version(output);
                    return output;
                }
                std::optional<parser_flag> fl{};
                for (auto &s : flags) {
                    if (s.long_k != lg) continue;

                    fl = s;
                    break;
                }
                if (!fl) {
                    output.valid = false;
                    output.err = std::format("Unknown long flag {}",lg);

                    break;
                }
                output.flags.insert(fl.value().short_k);

                continue;
            }

            for (size_t i = 1; i < cur.size(); i ++) {
                char c = cur[i];

                if (c == 'h') {
                    set_parser_help(output,flags,vals);
                    return output;
                }

                std::optional<parser_flag> fl{};
                for (auto &s : flags) {
                    if (s.short_k != c) continue;

                    fl = s;
                    break;
                }
                if (!fl) {
                    output.valid = false;
                    output.err = std::format("Unknown short flag {}",c);

                    return output;
                }
                output.flags.insert(fl.value().short_k);
            }
        }
        output.valid = true;
        output.can_proceed = true;

        return output;
    }

}

template<>
struct std::formatter<cbu::parser_output> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const cbu::parser_output& p, std::format_context& ctx) const {
        return std::format_to(ctx.out(),"{}",string(p));
    }
};
