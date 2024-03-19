#include <exception>
#include <functional>
#include <future>
#include <memory>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

template <typename Value>
class Future;

template <>
class Future<void> {
   private:
    std::shared_ptr<std::promise<void>> promise;
    std::shared_future<void> future;

    std::function<void(std::shared_future<void>)> continuation = [](std::shared_future<void>) {};

   public:
    DLL_EXPORT Future<void>() : promise(std::make_shared<std::promise<void>>()),
                                future(promise->get_future()) {
    }

    DLL_EXPORT void set() {
        promise->set_value();
        continuation(future);
    }

    DLL_EXPORT void set_exception(std::exception_ptr e) {
        promise->set_exception(e);
        continuation(future);
    }

    DLL_EXPORT void get() {
        future.get();
    }

    DLL_EXPORT void wait() {
        future.wait();
    }

    DLL_EXPORT void set_continuation(const std::function<void(std::shared_future<void>)>& func) {
        continuation = func;
    }
};

template <typename Value>
class Future {
   private:
    std::shared_ptr<std::promise<Value>> promise;
    std::shared_future<Value> future;

    std::function<void(std::shared_future<Value>)> continuation = [](std::shared_future<Value>) {};

   public:
    DLL_EXPORT Future<Value>() : promise(std::make_shared<std::promise<Value>>()),
                                 future(promise->get_future()) {
    }

    DLL_EXPORT void set(Value value) {
        promise->set_value(value);
        continuation(future);
    }

    DLL_EXPORT void set_exception(std::exception_ptr e) {
        promise->set_exception(e);
        continuation(future);
    }

    DLL_EXPORT Value get() {
        return future.get();
    }

    DLL_EXPORT void wait() {
        future.wait();
    }

    DLL_EXPORT void set_continuation(const std::function<void(std::shared_future<Value>)>& func) {
        continuation = func;
    }
};

template <typename Value>
class SharedFuture;

template <>
class SharedFuture<void> {
   private:
    std::shared_ptr<Future<void>> future;

   public:
    DLL_EXPORT SharedFuture() : future(std::make_shared<Future<void>>()) {
    }

    DLL_EXPORT std::shared_ptr<Future<void>> get_future() {
        return future;
    }

    DLL_EXPORT void set() {
        future->set();
    }

    DLL_EXPORT void set_exception(const std::exception_ptr& e) {
        future->set_exception(e);
    }

    DLL_EXPORT void get() {
        future->get();
    }

    DLL_EXPORT void wait() {
        future->wait();
    }

    /**
     * Executes a function after this future has been resolved.
     *
     * @param[in]   task    function to be executed.
     * @return shared future that will be result after task has finished.
     */
    DLL_EXPORT SharedFuture<void> then(std::function<void()> task) {
        SharedFuture<void> f;
        std::shared_ptr<Future<void>> fp = f.get_future();

        future->set_continuation([fp, task](std::shared_future<void> f1) {
            try {
                f1.get();
                task();
                fp->set();
            } catch (...) {
                fp->set_exception(std::current_exception());
            }
        });

        return f;
    }

    /**
     * Executes a function after this future has been resolved.
     *
     * @param[in]   task    function to be executed. The result of future is passed to task.
     * @return shared future that will contain the result produced by task.
     */
    template <typename Result>
    DLL_EXPORT SharedFuture<Result> then(std::function<Result()> task) {
        SharedFuture<Result> f;
        std::shared_ptr<Future<Result>> fp = f.get_future();

        future->set_continuation([fp, task](std::shared_future<void> f1) {
            try {
                f1.get();
                fp->set(task());
            } catch (...) {
                fp->set_exception(std::current_exception());
            }
        });

        return f;
    }
};

template <typename Value>
class SharedFuture {
   private:
    std::shared_ptr<Future<Value>> future;

   public:
    DLL_EXPORT SharedFuture() : future(std::make_shared<Future<Value>>()) {
    }

    DLL_EXPORT std::shared_ptr<Future<Value>> get_future() {
        return future;
    }

    DLL_EXPORT void set(Value value) {
        future->set(value);
    }

    DLL_EXPORT void set_exception(const std::exception_ptr& e) {
        future->set_exception(e);
    }

    DLL_EXPORT Value get() {
        return future->get();
    }

    DLL_EXPORT void wait() {
        future->wait();
    }

    /**
     * Executes a function after this future has been resolved.
     *
     * @param[in]   task    function to be executed.
     * @return shared future that will be result after task has finished.
     */
    DLL_EXPORT SharedFuture<void> then(std::function<void(Value)> task) {
        SharedFuture<void> f;
        std::shared_ptr<Future<void>> fp = f.get_future();

        future->set_continuation([fp, task](std::shared_future<Value> f1) {
            try {
                task(f1.get());
                fp->set();
            } catch (...) {
                fp->set_exception(std::current_exception());
            }
        });

        return f;
    }

    /**
     * Executes a function after this future has been resolved.
     *
     * @param[in]   task    function to be executed. The result of future is passed to task.
     * @return shared future that will contain the result produced by task.
     */
    template <typename Result>
    DLL_EXPORT SharedFuture<Result> then(std::function<Result(Value)> task) {
        SharedFuture<Result> f;
        std::shared_ptr<Future<Result>> fp = f.get_future();

        future->set_continuation([fp, task](std::shared_future<Value> f1) {
            try {
                fp->set(task(f1.get()));
            } catch (...) {
                fp->set_exception(std::current_exception());
            }
        });

        return f;
    }
};

}  // namespace DiscordCPP
