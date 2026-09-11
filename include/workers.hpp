#pragma once

#include "logger.hpp"
#include "math.hpp"
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
    using ThreadCount = uint16_t;

    struct WorkerState {
        std::counting_semaphore<> s = std::counting_semaphore<>(0);
        std::mutex lock{};
        volatile bool threads_quit = false;
        ThreadCount count = 0;
        size_t work_count = 0;
        std::condition_variable work_amount{};
        std::mutex work_mutex{};

        std::vector<std::thread> threads{};
        std::queue<WorkObject> sync_queue{};
        int sync_count = 0;
        std::queue<WorkObject> async_queue{};

        std::atomic<int> workers_work_done = 0;
        int workers_work_count = 0;
    };
    WorkerState *get_state();

    inline static void worker_pool_main() {
        cbu::log_verbose("Hello from worker thread!");
        auto state = get_state();
        while (true) {
            state->s.acquire();
            if (state->threads_quit) break;
            state->lock.lock();
            if (state->sync_count > 0) {
                WorkObject obj = state->sync_queue.front();
                state->sync_queue.pop();
                state->sync_count -= 1;

                // log_verbose(std::format("Running sync, {} left",sync_count));
                state->lock.unlock();

                obj.call();
                obj.finished();
            } else {
                WorkObject obj = state->async_queue.front();
                state->async_queue.pop();

                // log_verbose(std::format("Running async"));
                state->lock.unlock();

                obj.call();
                obj.finished();
            }
        }

        cbu::log_debug("Ending thread");
    }
    inline static void tasks_sync_cb() {
        auto state = get_state();
        std::unique_lock lock(state->work_mutex);
        state->work_count -= 1;
        if (state->work_count > 0) return;
        state->work_amount.notify_all();
    }
    inline void run_tasks_sync(std::vector<std::optional<WorkObject>> &objs) {
        auto state = get_state();
        state->lock.lock();
        size_t c = 0;
        for (std::optional<WorkObject> &obj : objs) {
            if (!obj) continue;

            state->sync_queue.push((WorkObject) {
                .call = obj->call,
                .finished = tasks_sync_cb,
            });
            state->work_count += 1;
            state->sync_count += 1;
            c += 1;
        }
        state->lock.unlock();
        if (c <= 0) return;
        state->s.release(c);

        std::unique_lock lk(state->work_mutex);
        state->work_amount.wait(lk,[&state]{ return state->work_count == 0; });
    }
    inline void run_tasks_async(std::vector<WorkObject> &objs,void (*cb)(void)) {
        auto state = get_state();
        cbu::log_verbose(std::format("Running {} tasks async, current tasks: {}/{}",objs.size(),state->workers_work_done.load(),state->workers_work_count));
        size_t c = 0;
        state->lock.lock();
        state->workers_work_done = 0;
        for (WorkObject &obj : objs) {
            state->async_queue.push((WorkObject) {
                .call = obj.call,
                .finished = [cb,&state]() {
                    // log_verbose(std::format("Work done: {}/{}",workers_work_done.load(),workers_work_count));

                    state->workers_work_done += 1;
                    if (state->workers_work_done < state->workers_work_count) return;

                    cb();
                }
            });
            c += 1;
        }
        state->workers_work_count = c;
        state->lock.unlock();

        state->s.release(c);
    }

    inline void start_worker_threads() {
        auto state = get_state();
        state->count = i_clamp(std::thread::hardware_concurrency(),4,64);
        state->threads.reserve(state->count);

        for (ThreadCount i = 0; i < state->count; i ++) {
            state->threads.emplace_back(std::thread(&worker_pool_main));
        }

        cbu::log_info(std::format("Started {} worker threads",state->count));
    }
    inline void quit_all_threads() {
        auto state = get_state();
        state->threads_quit = true;
        state->s.release(10000);
        for (ThreadCount i = 0; i < state->count; i ++) {
            std::thread &th = state->threads[i];
            if (!th.joinable()) continue;
            th.join();
        }
    }
}
