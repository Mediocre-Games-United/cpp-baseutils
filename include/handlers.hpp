#pragma once
#include <cstddef>

#if defined(_WIN32)

#define NOMINMAX
#include <windows.h>

namespace cbu {
    namespace interrupt_detail {
        inline volatile std::size_t* target = nullptr;

        inline BOOL WINAPI console_handler(DWORD signal)
        {
            if (signal == CTRL_C_EVENT && target != nullptr) {
                *target += 1;
                return TRUE;
            }

            return FALSE;
        }
    }


    inline void set_interrupt_handler(volatile std::size_t& target)
    {
        interrupt_detail::target = &target;
        SetConsoleCtrlHandler(interrupt_detail::console_handler, TRUE);
    }
}

#else

#include <csignal>

namespace cbu {
    namespace interrupt_detail {
        inline volatile std::size_t* target = nullptr;

        inline void signal_handler(int signal)
        {
            if (signal == SIGINT && target != nullptr) {
                *target += 1;
            }
        }
    }

    inline void set_interrupt_handler(volatile std::size_t& target)
    {
        interrupt_detail::target = &target;

        struct sigaction action {};
        action.sa_handler = interrupt_detail::signal_handler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;

        sigaction(SIGINT, &action, nullptr);
    }
}

#endif
