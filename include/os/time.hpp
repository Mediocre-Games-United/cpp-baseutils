#pragma once

#include "base_types.hpp"
#include <chrono>

class Timer {
public:
    Timer(Clock &c) : clock(c) {}

    F64 previous_delta() {
        TimePoint now = clock.now();
        std::chrono::duration<F64> diff = now - previous;
        previous = now;

        return diff.count();
    }
    void set_baseline() {
        previous = clock.now();
    }
private:
    Clock &clock;
    TimePoint previous;
};

extern Clock main_clock;
