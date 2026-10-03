#ifndef STRING_MATH_HPP
#define STRING_MATH_HPP

#include "base_types.hpp"

#include <algorithm>
#include <string_view>
#include <vector>

namespace cbu {
    class StringName {
    public:
        constexpr StringName() noexcept
        : hash_index(0) {}

        constexpr StringName(const char* c) noexcept
        : hash_index(hash(std::string_view{c})) {}

        constexpr StringName(std::string_view s) noexcept
        : hash_index(hash(s)) {}

        // Runtime-only convenience constructor
        StringName(const std::string& s) noexcept
        : hash_index(hash(s)) {}

        constexpr bool operator==(const StringName& other) const noexcept {
            return hash_index == other.hash_index;
        }

        std::size_t hash_index;
    private:

        static constexpr std::size_t hash(std::string_view s) noexcept {
            // FNV-1a constants for size_t-sized hashes
            constexpr std::size_t offset_basis =
            sizeof(std::size_t) == 8
            ? std::size_t{14695981039346656037ull}
            : std::size_t{2166136261u};

            constexpr std::size_t prime =
            sizeof(std::size_t) == 8
            ? std::size_t{1099511628211ull}
            : std::size_t{16777619u};

            std::size_t result = offset_basis;

            for (unsigned char ch : s) {
                result ^= ch;
                result *= prime;
            }

            return result;
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

    inline void trim(std::string& value)
    {
        while (!value.empty() &&
            std::isspace(static_cast<unsigned char>(value.back()))) {
            value.pop_back();
            }

            std::size_t first = 0;
        while (first < value.size() &&
            std::isspace(static_cast<unsigned char>(value[first]))) {
            ++first;
            }

            value.erase(0, first);
    }
}
template<>
struct std::hash<cbu::StringName> {
    size_t operator()(const cbu::StringName& s) const noexcept {
        return std::hash<size_t>{}(s.hash_index);
    }
};

#endif
