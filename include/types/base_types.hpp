#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <format>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <queue>

template <typename T>
using vector = std::vector<T>;
template <typename K,typename T>
using umap = std::unordered_map<K,T>;

template <typename T>
using queue = std::queue<T>;
template <typename T>
using func = std::function<T>;

using U64 = uint64_t;
using S64 = int64_t;
using U32 = uint32_t;
using S32 = int32_t;
using U16 = uint16_t;
using S16 = int16_t;
using U8 = uint8_t;
using S8 = int8_t;

using F32 = float;
using F64 = double;

using CLayer = U64;

# define PI          3.141592653589793238462643383279502884L /* pi */


typedef std::chrono::steady_clock Clock;
typedef Clock::time_point TimePoint;


namespace cbu {
    template <typename X,typename Base>
    inline bool is_of_type(Base *p) { assert(p); return dynamic_cast<X*>(p); }
}

#define VEC_ZERO vec(0,0)
