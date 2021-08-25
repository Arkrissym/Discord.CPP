#include "Logger.h"

#include <map>
#include <mutex>

static Loglevel _level = Debug;
static std::map<std::thread::id, std::string> thread_names;
static std::mutex log_mutex;

void Logger::register_thread(const std::thread::id& id, const std::string& name) {
    std::lock_guard<std::mutex> lock(log_mutex);
    thread_names.insert_or_assign(id, name);
}

void Logger::unregister_thread(const std::thread::id& id) {
    std::lock_guard<std::mutex> lock(log_mutex);
    thread_names.erase(id);
}

Logger::Logger(const std::string& name) {
    std::lock_guard<std::mutex> lock(log_mutex);
    _name = name;
}

void Logger::set_log_level(const Loglevel& level) { _level = level; }

void Logger::print(const Loglevel level, const std::string& message) {
    if (level >= _level) {
        std::string level_name;
        switch (level) {
            case Debug:
                level_name = "DEBUG";
                break;
            case Info:
                level_name = "INFO";
                break;
            case Warning:
                level_name = "WARNING";
                break;
            case Error:
                level_name = "ERROR";
                break;
            default:
                break;
        }

        std::lock_guard<std::mutex> lock(log_mutex);
        auto name = thread_names.find(std::this_thread::get_id());
        std::cout << level_name.c_str() << " - ";
        if (name != thread_names.end()) {
            std::cout << name->second;
        } else {
            std::cout << "Thread-" << std::this_thread::get_id();
        }
        std::cout << " - " << _name.c_str() << " - " << message.c_str() << std::endl;
    }
}

void Logger::debug(const std::string& message) { print(Debug, message); }

void Logger::info(const std::string& message) { print(Info, message); }

void Logger::warning(const std::string& message) { print(Warning, message); }

void Logger::error(const std::string& message) { print(Error, message); }
