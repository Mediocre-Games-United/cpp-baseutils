#pragma once

#include "worker_state.hpp"

namespace cbu {
    struct GlobalCBUState {
        WorkerState worker_state{};
        std::mutex cli_mutex{};
        std::mutex logger_mutex{};
#ifdef VERBOSE
        bool verbose = true;
#else
        bool verbose = false;
#endif
    };

    extern GlobalCBUState global_state;
}
