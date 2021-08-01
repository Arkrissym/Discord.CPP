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

#define MAX_QUEUE_SIZE 128

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

    DLL_EXPORT void start_thread();
    DLL_EXPORT void queue_task(const std::function<void()>& task);

   public:
    DLL_EXPORT Threadpool(const unsigned int size = 1);
    Threadpool(const Threadpool&) = delete;
    DLL_EXPORT ~Threadpool();

    template <typename F, typename R = std::invoke_result_t<std::decay_t<F>>, typename = std::enable_if_t<!std::is_void_v<R>>>
    DLL_EXPORT std::future<R> execute(F&& task) {
        std::shared_ptr<std::promise<R>> promise(new std::promise<R>);
        std::future<R> future = promise->get_future();

        auto t = std::make_shared<typename std::remove_reference<F>::type>(std::forward<F>(task));

        queue_task([this, promise, t]() {
            log.debug("executing task");
            try {
                promise->set_value((*t)());
                log.debug("finished task");
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
        });

        return future;
    }

    template <typename F, typename = std::enable_if_t<std::is_void_v<std::invoke_result_t<std::decay_t<F>>>>>
    DLL_EXPORT std::future<void> execute(F&& task) {
        std::shared_ptr<std::promise<void>> promise(new std::promise<void>);
        std::future<void> future = promise->get_future();

        auto t = std::make_shared<typename std::remove_reference<F>::type>(std::forward<F>(task));

        queue_task([this, promise, t]() {
            try {
                log.debug("executing task 2");
                (*t)();
                log.debug("finished task 2");
                promise->set_value();
                log.debug("resolved promise 2");
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
        });

        return future;
    }

    template <typename F, typename T, typename R = std::invoke_result_t<std::decay_t<T>, std::decay_t<F>>>
    DLL_EXPORT std::future<R> then(std::future<F>& future, T&& task) {
        //DLL_EXPORT auto then(const std::future<F>& future, const T& task) -> std::future<decltype(task(future.get()))> {
        log.debug("Future valid? " + std::to_string(future.valid()));
        auto f = std::make_shared<std::future<F>>(std::move(future));
        auto t = std::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(task));
        return execute([f, t]() {
            return (*t)(f->get());
        });
    }
    template <typename T, typename R = std::invoke_result_t<std::decay_t<T>>>
    DLL_EXPORT std::future<R> then(std::future<void>& future, T&& task) {
        //DLL_EXPORT auto then(const std::future<void>& future, const T& task) -> std::future<decltype(task())> {
        log.debug("Future valid? " + std::to_string(future.valid()));
        auto f = std::make_shared<std::future<void>>(std::move(future));
        auto t = std::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(task));
        return execute([f, t]() {
            f->wait();
            return (*t)();
        });
    }
};

}  // namespace DiscordCPP
