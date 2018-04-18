#pragma once
#include <iostream>

enum Loglevel {
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

class Logger {
protected:
	std::string _name;
	Loglevel _level = DEBUG;
public:
	__declspec(dllexport) Logger();
	__declspec(dllexport) ~Logger();
	__declspec(dllexport) Logger(std::string);
	__declspec(dllexport) Logger(const Logger &old);

	__declspec(dllexport) void set_log_level(Loglevel level);

	__declspec(dllexport) void print(Loglevel level, std::string message);

	__declspec(dllexport) void debug(std::string message);
	__declspec(dllexport) void info(std::string message);
	__declspec(dllexport) void warning(std::string message);
	__declspec(dllexport) void error(std::string message);
};

