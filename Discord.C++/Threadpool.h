#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

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
    std::mutex mutex;
    std::queue<std::function<void()>> task_queue;
    std::condition_variable condition;
    bool shutdown = false;
    unsigned int thread_count = 0;
    unsigned int threadpool_id;
    Logger log;

    DLL_EXPORT void start_thread();

   public:
    DLL_EXPORT Threadpool(const unsigned int size = 2);
    Threadpool(const Threadpool&) = delete;
    DLL_EXPORT ~Threadpool();

    DLL_EXPORT void execute(const std::function<void()>& task);
};

}  // namespace DiscordCPP
