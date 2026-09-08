#pragma once

#include "logger.hpp"
#include "workers.hpp"

namespace cbu {
    inline void init() {
        cbu::log_debug("Starting C baseutills");
        start_worker_threads();
        cbu::log_success("C baseutils succesfully started");
    }
    inline void deinit() {
        cbu::log_debug("Stopping C baseutills...");
        quit_all_threads();
    }
};
