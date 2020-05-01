#pragma once
#include <iostream>

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

class Logger {
protected:
	std::string _name;
public:
	DLL_EXPORT Logger();
	DLL_EXPORT ~Logger();
	DLL_EXPORT Logger(const std::string&);
	DLL_EXPORT Logger(const Logger& old);

	DLL_EXPORT void set_log_level(const Loglevel& level);

	DLL_EXPORT void print(const Loglevel level, const std::string& message);

	DLL_EXPORT void debug(const std::string& message);
	DLL_EXPORT void info(const std::string& message);
	DLL_EXPORT void warning(const std::string& message);
	DLL_EXPORT void error(const std::string& message);
};

