#ifndef STRING_MATH_HPP
#define STRING_MATH_HPP

#include "base_types.hpp"

#include <algorithm>
#include <string_view>
#include <vector>

namespace baseutils {
    class StringName {
    public:
        StringName() = default;
        StringName(const char *c) {
            hash_index = hash(c);
        }
        StringName(string c) {
            hash_index = hash(c.c_str());
        }
        size_t hash_index;

        inline bool operator==(const StringName &other) const { return hash_index == other.hash_index; }
    private:
        inline size_t hash(const char *c) {
            return std::hash<std::string_view>{}(std::string_view{c});
        }
    };

    inline string string_uppercase(string s) {
        std::transform(s.begin(),s.end(),s.begin(),[](letter c) {
            return std::toupper(c);
        });
        return s;
    }
    inline std::vector<string> string_split(string s,string splitter) {
        std::vector<string> res;

        size_t p = 0;
        size_t i = 0;
        size_t si = 0;
        size_t sc = splitter.size();
        for (letter c : s) {
            if (c != splitter[si]) {
                i += 1;
                si = 0;
                continue;
            }

            si += 1;
            i += 1;
            if (si < sc) continue;

            res.push_back(s.substr(p,i - p - 1));
            p = i;
        }
        if (p < i) res.push_back(s.substr(p,i - p));

        return res;
    }
}
template<>
struct std::hash<baseutils::StringName> {
    size_t operator()(const baseutils::StringName& s) const noexcept {
        return std::hash<size_t>{}(s.hash_index);
    }
};

#endif
