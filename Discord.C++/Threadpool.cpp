#include "Threadpool.h"

DiscordCPP::Threadpool::Threadpool(const unsigned int size) {
    static unsigned int thread_pool_index = 0;
    threadpool_id = thread_pool_index++;

    log = Logger("Threadpool-" + std::to_string(threadpool_id));

    for (unsigned int i = 0; i < size; i++) {
        start_thread();
    }
}

DiscordCPP::Threadpool::~Threadpool() {
    {
        std::lock_guard<std::mutex> lock(state_mutex);
        shutdown = true;
    }
    condition.notify_all();
}

void DiscordCPP::Threadpool::start_thread() {
    std::lock_guard<std::mutex> lock(state_mutex);

    std::thread thread = std::thread(
        [this]() {
            Logger thread_log = Logger("Worker");

            thread_log.debug("Thread started");

            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    condition.wait(lock, [this]() {
                        return shutdown || !task_queue.empty();
                    });
                    if (shutdown) {
                        break;
                    }

                    task = std::move(task_queue.front());
                    task_queue.pop();
                }

                task();
            }

            thread_log.debug("Thread closed");
            Logger::unregister_thread(std::this_thread::get_id());
        });

    Logger::register_thread(thread.get_id(), "Threadpool-" + std::to_string(threadpool_id) +
                                                 "-Thread-" + std::to_string(thread_count));

    thread.detach();

    thread_count++;
}

void DiscordCPP::Threadpool::queue_task(const std::function<void()>& task) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    if (shutdown) {
        throw std::runtime_error("Cannot execute task on stopped threadpool");
    }

    if (task_queue.size() > 0 && thread_count < std::thread::hardware_concurrency()) {
        log.debug("starting new thread");
        start_thread();
    }

    task_queue.emplace(task);
    condition.notify_one();
}
