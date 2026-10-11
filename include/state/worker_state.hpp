#pragma once

#include "base_types.hpp"
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include <format>

namespace cbu {
    using WorkerFunction = std::function<void()>;
    using AsyncCallback = void (*)(int step,int max);
    struct WorkObject {
        WorkerFunction call;
        WorkerFunction finished = NULL;
    };
    using ThreadCount = U16;

    struct WorkerState {
        std::counting_semaphore<> s{0};
        std::mutex lock{};
        volatile bool threads_quit = false;
        ThreadCount count = 0;
        size_t work_count = 0;
        std::condition_variable work_amount{};
        std::mutex work_mutex{};

        vector<std::thread> threads{};
        std::queue<WorkObject> sync_queue{};
        int sync_count = 0;
        std::queue<WorkObject> async_queue{};

        std::atomic<int> workers_work_done = 0;
        int workers_work_count = 0;

        inline void sync_pre() {
            lock.lock();
        }
        inline void sync_post(size_t c) {
            work_count += c;
            sync_count += c;
            lock.unlock();
            if (c <= 0) return;

            s.release(c);

            std::unique_lock lk(work_mutex);
            work_amount.wait(lk,[this]{ return work_count == 0; });
        }
    };
}
