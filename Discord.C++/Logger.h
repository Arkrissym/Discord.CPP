#pragma once
#include <iostream>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

enum Loglevel {
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

class Logger {
protected:
	std::string _name;
public:
	DLL_EXPORT Logger();
	DLL_EXPORT ~Logger();
	DLL_EXPORT Logger(std::string);
	DLL_EXPORT Logger(const Logger &old);

	DLL_EXPORT void set_log_level(Loglevel level);

	DLL_EXPORT void print(Loglevel level, std::string message);

	DLL_EXPORT void debug(std::string message);
	DLL_EXPORT void info(std::string message);
	DLL_EXPORT void warning(std::string message);
	DLL_EXPORT void error(std::string message);
};

