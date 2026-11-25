#include "logger.hpp"
#include "ansi.hpp"
#include "cbu_state.hpp"
#include "file.hpp"
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <mutex>
#include "compress.hpp"

#ifdef CBU_LOGGER_ENABLE_MUTEX
#define LOGGER_GET_MUTEX std::lock_guard<std::mutex> lock(global_state.logger_mutex)
#endif
#define CBU_LOGGER_ENABLED if (!global_state.logger_enabled) return
#define CBU_LOGGER_IMPORTANT if (global_state.logger_only_important) return

#ifndef LOGGER_GET_MUTEX
#define LOGGER_GET_MUTEX //
#endif
#ifndef CBU_LOGGER_ENABLED
#define CBU_LOGGER_ENABLED //
#endif
#ifndef CBU_LOGGER_IMPORTANT
#define CBU_LOGGER_IMPORTANT //
#endif


static std::thread log_thread;
static bool log_running = false;
static std::mutex log_buffer_mutex;
static std::string log_buffer;
static std::fstream log_file_stream;

static void log_thread_call() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::duration<F64>(2));

        {
            std::lock_guard guard(log_buffer_mutex);
            if (!log_running and log_buffer.empty()) {
                return;
            }

            if (log_buffer.empty()) return;

            log_file_stream << log_buffer;
            log_file_stream.flush();
            log_buffer.clear();
        }
    }
}
static void log_txt(std::string bf) {
    std::lock_guard guard(log_buffer_mutex);

    if (!log_running) return;
    log_buffer += bf;
}

static std::string log_name;
static fpath latest;

void cbu::init_log_saving(std::string id) {
    log_running = true;
    log_buffer.clear();
    if (id.empty()) log_name = "log";
    else log_name = std::format("log_{}",id);

    fpath logpath = cbu::get_parent_dir() / "logs";
    latest = logpath / std::format("latest_{}.log",log_name);

    std::filesystem::create_directories(logpath);
    log_file_stream = std::fstream(latest.c_str(),std::ios_base::out | std::ios_base::trunc);

    log_thread = std::thread{log_thread_call};
}
void cbu::finish_log_saving() {
    {
        std::lock_guard guard(log_buffer_mutex);
        log_running = false;
    }
    log_thread.join();

    log_file_stream.flush();
    log_file_stream.close();

    auto now = std::chrono::system_clock::now();
    fpath name = cbu::get_parent_dir() / "logs" / std::format("{}_{}.log.xz",log_name,now);

    BYTEARRAY stream = cbu::require_file_binary_path(latest);
    BYTEARRAY comp = cbu::compress::xz::compress(stream,6);
    cbu::write_to_file_safe_binary(name,comp);
}

namespace cbu::detail {
    inline void print_log(const char* color, const char* label, const std::string& txt) {
        log_txt(std::format("{}: {}\n",label,txt));

        if (cbu::detail::colors_enabled() && color[0] != '\0') {
            std::printf("%s%s: %s\033[0m\n", color, label, txt.c_str());
        } else {
            std::printf("%s: %s\n", label, txt.c_str());
        }
    }
}  // namespace detail

void cbu::log_error(bool will_throw, std::string txt) {
    {
        LOGGER_GET_MUTEX;
        detail::print_log("\033[31;1m", "ERROR", txt);
    }
    if (will_throw) {
        throw std::runtime_error(txt.c_str());
    }
}

void cbu::log_warn(std::string txt) {
    CBU_LOGGER_ENABLED;
    LOGGER_GET_MUTEX;
    detail::print_log("\033[33;1m", "WARN", txt);
}

void cbu::log_info(std::string txt) {
    CBU_LOGGER_ENABLED;
    CBU_LOGGER_IMPORTANT;

    LOGGER_GET_MUTEX;
    detail::print_log("", "INFO", txt);
}

void cbu::log_network(std::string txt) {
    CBU_LOGGER_ENABLED;
    CBU_LOGGER_IMPORTANT;

    LOGGER_GET_MUTEX;
    detail::print_log("\033[35;1m", "NETWORK", txt);
}

void cbu::log_success(std::string txt) {
    CBU_LOGGER_ENABLED;
    LOGGER_GET_MUTEX;
    detail::print_log("\033[32;1m", "OK", txt);
}

void cbu::log_debug(std::string txt) {
    if (!global_state.debug) return;
    CBU_LOGGER_ENABLED;
    CBU_LOGGER_IMPORTANT;

    LOGGER_GET_MUTEX;
    detail::print_log("\033[36m", "DEBUG", txt);
}

void cbu::log_verbose(std::string txt) {
    if (!global_state.verbose) return;
    CBU_LOGGER_ENABLED;
    CBU_LOGGER_IMPORTANT;

    LOGGER_GET_MUTEX;
    detail::print_log("\033[34m", "VERBOSE", txt);
}

