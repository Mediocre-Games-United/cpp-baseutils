#pragma once

#include <string>
namespace cbu {
    void init_log_saving(std::string id = "");
    void finish_log_saving();

    void log_error(bool will_throw, std::string txt);
    void log_warn(std::string txt);
    void log_info(std::string txt);
    void log_debug(std::string txt);
    void log_network(std::string txt);
    void log_success(std::string txt);
    void log_verbose(std::string txt);
}  // namespace cbu
