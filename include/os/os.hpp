#include "base_types.hpp"

#include <cstdint>
#include <format>
#include "stringmath.hpp"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#else
#include <unistd.h>
#include <cstdio>
#endif

inline U64 os_process_memory_usage() {
    #if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS_EX pmc{};
    // RSS-like metric on Windows: WorkingSetSize (resident in RAM, approximate).
    if (!GetProcessMemoryInfo(GetCurrentProcess(),
        reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
                              sizeof(pmc))) {
        return 0;
                              }
                              return static_cast<U64>(pmc.WorkingSetSize);
                              #else
                              // Linux/Android: /proc/self/statm resident pages * page size
                              FILE* f = std::fopen("/proc/self/statm", "r");
                              if (!f) return 0;

                              unsigned long sizePages = 0, residentPages = 0;
    if (std::fscanf(f, "%lu %lu", &sizePages, &residentPages) != 2) {
        std::fclose(f);
        return 0;
    }
    std::fclose(f);

    long pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize <= 0) return 0;

    return static_cast<U64>(residentPages) * static_cast<U64>(pageSize);
    #endif
}
inline string os_format_memory(U64 mem) {
    if (mem > 1000000000) {
        return std::format("{:.2f} GB",mem / 1000000000.0);
    } else if (mem > 1000000) {
        return std::format("{:.2f} MB",mem / 1000000.0);
    } else if (mem > 1000) {
        return std::format("{:.2f} KB",mem / 1000.0);
    } else {
        return std::format("{} B",mem);
    }
}

const F64 year_time_delta = 3.15576e+7;
const F64 day_time_delta = 86400;
const F64 hour_time_delta = 3600;
const F64 minute_time_delta = 60;

inline static string format_sub_time(F64 time) {
    if (time < 0.0001) return std::format("{:.1f}us",time * 1000000.0);
    if (time < 0.1) return std::format("{:.2f}ms",time * 1000.0);

    return std::format("{:.2f}s",time);
}
inline string os_format_time(F64 time) {
    if (time <= 0) return "0s";
    if (time <= 1) return format_sub_time(time);

    U64 years = floor(time / year_time_delta);
    time -= years * year_time_delta;
    U64 days = floor(time / day_time_delta);
    time -= days * day_time_delta;
    U64 hours = floor(time / hour_time_delta);
    time -= hours * hour_time_delta;
    U64 minutes = floor(time / minute_time_delta);
    time -= minutes * minute_time_delta;

    if (years > 0) {
        return std::format("{}y {}d",years,days);
    }
    if (days > 0) {
        return std::format("{}d {}h",days,hours);
    }
    string ret = "";
    if (hours > 0) {
        ret += std::format("{}h",hours);
    }
    if (minutes > 0) {
        if (!ret.empty()) ret += " ";
        ret += std::format("{}m",minutes);
    }
    if (!ret.empty()) ret += " ";
    ret += std::format("{:.1f}s",time);

    return ret;
}
