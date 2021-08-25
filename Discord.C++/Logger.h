#pragma once

#include <iostream>
#include <thread>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

enum Loglevel {
    Debug,
    Info,
    Warning,
    Error
};

namespace DiscordCPP {
class Threadpool;
}

class Logger {
   protected:
    std::string _name;

   public:
    DLL_EXPORT static void register_thread(const std::thread::id& id, const std::string& name);
    DLL_EXPORT static void unregister_thread(const std::thread::id& id);

    DLL_EXPORT static void set_log_level(const Loglevel& level);

    DLL_EXPORT Logger(){};
    DLL_EXPORT Logger(const std::string&);

    DLL_EXPORT void print(const Loglevel level, const std::string& message);

    DLL_EXPORT void debug(const std::string& message);
    DLL_EXPORT void info(const std::string& message);
    DLL_EXPORT void warning(const std::string& message);
    DLL_EXPORT void error(const std::string& message);
};
