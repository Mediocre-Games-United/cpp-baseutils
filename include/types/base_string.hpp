#pragma once

#include "base_types.hpp"
#include <filesystem>
#include <format>
#include <string_view>
#include <string>
#include "logger.hpp"
#include "math.hpp"

using letter = char32_t;

namespace cbu {
    class string {
    public:
        string() {}
        string(std::string str) {
            try {
                data = from_utf8(str);
            } catch (error err) {
                cbu::log_error(false,std::format("string error: {}",err.what()));

                data = {};
            }
            copy = to_utf8();
        }
        string(const char *c) {
            try {
                data = from_utf8(c);
            } catch (error err) {
                cbu::log_error(false,std::format("string error: {}",err.what()));

                data = {};
            }
            copy = to_utf8();
        }
        string(letter c) {
            data = {c};
            try {
                copy = to_utf8();
            } catch (error err) {
                cbu::log_error(false,std::format("string error: {}",err.what()));

                data = {};
                copy = {};
            }
        }
        string(vector<letter> d) {
            data = d;
            try {
                copy = to_utf8();
            } catch (error err) {
                cbu::log_error(false,std::format("string error: {}",err.what()));

                data = {};
                copy = {};
            }
        }
        bool operator==(const string &other) const {
            if (other.size() != size()) return false;

            for (size_t i = 0; i < size(); i ++) {
                if (data[i] == other.data[i]) continue;

                return false;
            }

            return true;
        }
        bool operator==(const char *other) const {
            return *this == string(other);
        }
        letter operator[](size_t i) const {
            cbu::log_debug(std::format("Get index {} size {}",i,size()));

            return data[i];
        }

        friend string operator+(const string &th,const string &other) {
            auto copy = th.data;
            copy.insert(copy.end(),other.data.begin(),other.data.end());

            return copy;
        }
        inline string &operator+=(const string &other) {
            data.insert(data.end(),other.data.begin(),other.data.end());
            try {
                copy = to_utf8();
            } catch (error err) {
                cbu::log_error(false,std::format("string error: {}",err.what()));

                data = {};
                copy = {};
            }

            return *this;
        }

        operator std::string() const {
            try {
                return to_utf8();
            } catch (error err) {
                cbu::log_error(false,std::format("string error: {}",err.what()));
                return {};
            }
        }
        operator std::filesystem::path() const {
            try {
                return to_utf8();
            } catch (error err) {
                cbu::log_error(false,std::format("string error: {}",err.what()));
                return {};
            }
        }
        operator const char *() const {
            return copy.c_str();
        }


        struct error {
            error(std::string err) : self(err) {}
            std::string what() {
                return self;
            }
        private:
            std::string self;
        };

        inline string to_uppercase() {
            string n = *this;

            std::transform(n.data.begin(),n.data.end(),n.data.begin(),[](letter c) {
                return std::toupper(c);
            });

            return n.data;
        }
        inline string substr(size_t start,size_t len = -1) const {
            return vector<letter>(data.begin() + start,data.begin() + start + cbu::min(len,size() - start));
        }
        inline bool starts_with(string other) const {
            string sub = substr(0,other.size());
            return sub == other;
        }
        inline bool end_with(string other) const {
            string sub = substr(size() - other.size(),other.size());
            return sub == other;
        }

        inline vector<string> split(string splitter) {
            vector<string> res;

            size_t p = 0;
            size_t i = 0;
            size_t si = 0;
            size_t sc = splitter.size();
            for (letter c : data) {
                if (c != splitter.data[si]) {
                    i += 1;
                    si = 0;
                    continue;
                }

                si += 1;
                i += 1;
                if (si < sc) continue;

                res.push_back(substr(p,i - p - 1));
                p = i;
            }
            if (p < i) res.push_back(substr(p,i - p));

            return res;
        }

        inline void trim()
        {
            while (!data.empty() &&
                std::isspace(data.back())) {
                data.pop_back();
                }

                std::size_t first = 0;
            while (first < data.size() &&
                std::isspace(data[first])) {
                ++first;
                }

                data = vector<char32_t>{data.begin() + first,data.end()};

            try {
                copy = to_utf8();
            } catch (error err) {
                cbu::log_error(false,std::format("string error: {}",err.what()));

                data = {};
                copy = {};
            }
        }


        inline bool empty() const {
            return data.empty();
        }
        inline size_t size() const {
            return data.size();
        }
        inline void insert(size_t index,string sub) {
            cbu::log_debug(std::format("Pre insert index {} size {} insert size {}",index,size(),sub.size()));
            data.insert(data.begin() + index,sub.data.begin(),sub.data.end());
            cbu::log_debug(std::format("Post insert size {}",size()));
        }
        inline void erase(size_t index,size_t len) {
            data.erase(data.begin() + index, data.begin() + index + len);
        }

        using iterator = typename vector<letter>::iterator;
        using const_iterator = typename vector<letter>::const_iterator;
        inline iterator begin() {
            return data.begin();
        }
        inline iterator end() {
            return data.end();
        }
        inline const_iterator begin() const {
            return data.begin();
        }
        inline const_iterator end() const {
            return data.end();
        }
        std::string to_utf8() const {
            std::string output;

            for (char32_t cp : data) {
                // UTF-32 values must be Unicode scalar values.
                if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF))
                    throw error("Invalid Unicode code point");

                if (cp <= 0x7F) {
                    output.push_back(static_cast<char>(cp));
                } else if (cp <= 0x7FF) {
                    output.push_back(static_cast<char>(0xC0 | (cp >> 6)));
                    output.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                } else if (cp <= 0xFFFF) {
                    output.push_back(static_cast<char>(0xE0 | (cp >> 12)));
                    output.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                    output.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                } else {
                    output.push_back(static_cast<char>(0xF0 | (cp >> 18)));
                    output.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
                    output.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                    output.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                }
            }

            return output;
        }
    private:
        inline vector<letter> from_utf8(std::string in) {
            vector<letter> output{};

            output.reserve(in.size());

            const auto* bytes =
            reinterpret_cast<const unsigned char*>(in.data());

            for (std::size_t i = 0; i < in.size();) {
                const unsigned char first = bytes[i++];
                char32_t codepoint;
                std::size_t continuationCount;

                if (first <= 0x7F) {
                    codepoint = first;
                    continuationCount = 0;
                } else if (first >= 0xC2 && first <= 0xDF) {
                    codepoint = first & 0x1F;
                    continuationCount = 1;
                } else if (first >= 0xE0 && first <= 0xEF) {
                    codepoint = first & 0x0F;
                    continuationCount = 2;
                } else if (first >= 0xF0 && first <= 0xF4) {
                    codepoint = first & 0x07;
                    continuationCount = 3;
                } else {
                    throw error("Invalid UTF-8 leading byte");
                }

                if (in.size() - i < continuationCount)
                    throw error("Truncated UTF-8 sequence");

                for (std::size_t j = 0; j < continuationCount; ++j) {
                    const unsigned char next = bytes[i++];

                    if ((next & 0xC0) != 0x80)
                        throw error("Invalid UTF-8 continuation byte");

                    codepoint = (codepoint << 6) | (next & 0x3F);
                }

                // Reject overlong encodings, surrogate code points, and values
                // outside the Unicode range.
                if ((continuationCount == 1 && codepoint < 0x80) ||
                    (continuationCount == 2 && codepoint < 0x800) ||
                    (continuationCount == 3 && codepoint < 0x10000) ||
                    (codepoint >= 0xD800 && codepoint <= 0xDFFF) ||
                    codepoint > 0x10FFFF) {
                    throw error("Invalid UTF-8 sequence");
                    }

                    output.push_back(codepoint);
            }

            return output;
        }
        vector<letter> data;
        std::string copy;
    };
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
        StringName(const string& s) noexcept
        : hash_index(hash(s.to_utf8())) {}

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

    template<typename T>
    inline string string_n(string single,string many,T c) {
        if (c == 1) return single;

        return many;
    }
}

using string = cbu::string;
using stringname = cbu::StringName;
template<>
struct std::hash<stringname> {
    size_t operator()(const stringname& s) const noexcept {
        return std::hash<size_t>{}(s.hash_index);
    }
};
template<>
struct std::hash<string> {
    size_t operator()(const string& s) const noexcept {
        return std::hash<stringname>{}(s);
    }
};

template<>
struct std::formatter<string> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const string& f, std::format_context& ctx) const {
        return std::format_to(ctx.out(),"{}",f);
    }
};


