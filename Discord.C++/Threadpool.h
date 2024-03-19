#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

#include "Future.h"
#include "Logger.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

class Threadpool {
   private:
    std::mutex queue_mutex;
    std::queue<std::function<void()>> task_queue;
    std::condition_variable condition;

    std::mutex state_mutex;
    bool shutdown = false;
    std::vector<std::thread> threads;
    unsigned int thread_count = 0;
    unsigned int threadpool_id;
    Logger log;

    /// Starts a new worker thread used by the Threadpool.
    DLL_EXPORT void start_thread();
    /// Adds a task to the queue to be processed by one of the worker threads.
    DLL_EXPORT void queue_task(const std::function<void()>& task);

   public:
    /**
     *  Creates a new Threadpool.
     *
     *  @param[in]  size    (optional) The number of threads the threadpool should contain. Defaults to 4.
     */
    DLL_EXPORT explicit Threadpool(const unsigned int size = 4);
    Threadpool(const Threadpool&) = delete;
    DLL_EXPORT ~Threadpool();

    /**
     * Executes a function on the Threadpool.
     *
     * @param[in]   task    function to be executed.
     * @return shared future that will contain the result produced by task.
     */
    template <typename F, typename R = std::invoke_result_t<std::decay_t<F>>, typename = std::enable_if_t<!std::is_void_v<R>>>
    DLL_EXPORT SharedFuture<R> execute(F&& task) {
        SharedFuture<R> future;
        auto fp = future.get_future();

        auto t = std::make_shared<typename std::remove_reference<F>::type>(std::forward<F>(task));

        queue_task([fp, t]() {
            try {
                fp->set((*t)());
            } catch (...) {
                fp->set_exception(std::current_exception());
            }
        });

        return future;
    }

    /**
     * Executes a function on the Threadpool.
     *
     * @param[in]   task    function to be executed.
     * @return shared future that will contain the result produced by task.
     */
    template <typename F, typename = std::enable_if_t<std::is_void_v<std::invoke_result_t<std::decay_t<F>>>>>
    DLL_EXPORT SharedFuture<void> execute(F&& task) {
        SharedFuture<void> future;
        auto fp = future.get_future();

        auto t = std::make_shared<typename std::remove_reference<F>::type>(std::forward<F>(task));

        queue_task([fp, t]() {
            try {
                (*t)();
                fp->set();
            } catch (...) {
                fp->set_exception(std::current_exception());
            }
        });

        return future;
    }
};

}  // namespace DiscordCPP
