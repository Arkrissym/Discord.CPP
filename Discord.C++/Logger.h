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
	Loglevel _level;
public:
	Logger() {};
	Logger(std::string);
	void set_log_level(Loglevel level);

	void print(Loglevel level, std::string message);

	void debug(std::string message);
	void info(std::string message);
	void warning(std::string message);
	void error(std::string message);
};

