#ifdef IS_IN_TEST

#include "cbu_state.hpp"
#include "logger.hpp"
#include "time.hpp"
#include "unit_test.hpp"
#include "base_types.hpp"
#include "find_config.h"
#include <format>
#include "os.hpp"

using namespace cbu;
using namespace test;

Clock main_clock{};

int main(int argc,char **argv) {
    test_result res{};

    cbu::log_info(std::format("Running test {}...",CURRENT_TEST_NAME));
    global_state.logger_enabled = false;
    test_main(res,argc,argv);
    global_state.logger_enabled = true;

    F64 time = res.get_runtime();
    if (!res.success) {
        cbu::log_error(false,std::format("Test " CURRENT_TEST_NAME " failed:\n'{}'",res.err));

        return 1;
    }

    cbu::log_success(std::format("Test " CURRENT_TEST_NAME " succesful in {}!",os_format_time(time)));
    return 0;
}

#endif
