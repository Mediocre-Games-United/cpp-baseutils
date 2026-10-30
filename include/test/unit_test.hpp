#pragma once

#include "base_types.hpp"
#include "time.hpp"
#include <chrono>

#ifndef CURRENT_TEST_NAME
#define CURRENT_TEST_NAME "<unknown>"
#endif

namespace cbu::test {
    struct test_result {
        test_result() { run_timer.set_baseline(); }

        void set_success() {
            success = true;
        }
        void set_fail(string e) {
            success = false;
            err = e;
        }
        F64 get_runtime() { return run_timer.previous_delta(); }

        bool success;
        string err;
    private:
        Timer run_timer = {main_clock};
    };

    void test_main(test_result &res,int argc,char **argv);
}
