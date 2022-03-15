#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>

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
    DLL_EXPORT Threadpool(const unsigned int size = 4);
    Threadpool(const Threadpool&) = delete;
    DLL_EXPORT ~Threadpool();

    /**
     * Executes a function on the Threadpool.
     *
     * @param[in]   task    function to be executed.
     * @return shared future that will contain the result produced by task.
     */
    template <typename F, typename R = std::invoke_result_t<std::decay_t<F>>, typename = std::enable_if_t<!std::is_void_v<R>>>
    DLL_EXPORT std::shared_future<R> execute(F&& task) {
        std::shared_ptr<std::promise<R>> promise(new std::promise<R>);
        std::shared_future<R> future(promise->get_future());

        auto t = std::make_shared<typename std::remove_reference<F>::type>(std::forward<F>(task));

        queue_task([promise, t]() {
            try {
                promise->set_value((*t)());
            } catch (...) {
                promise->set_exception(std::current_exception());
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
    DLL_EXPORT std::shared_future<void> execute(F&& task) {
        std::shared_ptr<std::promise<void>> promise(new std::promise<void>);
        std::shared_future<void> future(promise->get_future());

        auto t = std::make_shared<typename std::remove_reference<F>::type>(std::forward<F>(task));

        queue_task([promise, t]() {
            try {
                (*t)();
                promise->set_value();
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
        });

        return future;
    }

    /**
     * Executes a function on the Threadpool after future has been resolved.
     *
     * @param[in]   future  shared future that will be awaited first.
     * @param[in]   task    function to be executed. The result of future is passed to task.
     * @return shared future that will contain the result produced by task.
     */
    template <typename F, typename T, typename R = std::invoke_result_t<std::decay_t<T>, std::decay_t<F>>>
    DLL_EXPORT std::shared_future<R> then(std::shared_future<F>& future, T&& task) {
        auto f = std::make_shared<std::shared_future<F>>(std::move(future));
        auto t = std::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(task));

        return execute([f, t]() {
            return (*t)(f->get());
        });
    }

    /**
     * Executes a function on the Threadpool after future has been resolved.
     *
     * @param[in]   future  shared future that will be awaited first.
     * @param[in]   task    function to be executed.
     * @return shared future that will contain the result produced by task.
     */
    template <typename T, typename R = std::invoke_result_t<std::decay_t<T>>>
    DLL_EXPORT std::shared_future<R> then(std::shared_future<void>& future, T&& task) {
        auto f = std::make_shared<std::shared_future<void>>(std::move(future));
        auto t = std::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(task));

        return execute([f, t]() {
            f->wait();
            return (*t)();
        });
    }
};

}  // namespace DiscordCPP
