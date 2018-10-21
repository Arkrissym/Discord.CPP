#include "Logger.h"

using namespace std;

static Loglevel _level = DEBUG;

Logger::Logger() {

}

Logger::~Logger() {

}

Logger::Logger(std::string name) {
	_name = name;
}

Logger::Logger(const Logger & old) {
	_name = old._name;
	//_level = old._level;
}

void Logger::set_log_level(Loglevel level) {
	_level = level;
}

void Logger::print(Loglevel level, std::string message) {
	if (level >= _level) {
		string level_name;
		switch (level) {
			case DEBUG:
				level_name = "DEBUG";
				break;
			case INFO:
				level_name = "INFO";
				break;
			case WARNING:
				level_name = "WARNING";
				break;
			case ERROR:
				level_name = "ERROR";
				break;
			default:
				break;
		}

		std::cout << level_name.c_str() << " - " << _name.c_str() << " - " << message.c_str() << endl;
	}
}

void Logger::debug(std::string message) {
	print(DEBUG, message);
}

void Logger::info(std::string message) {
	print(INFO, message);
}

void Logger::warning(std::string message) {
	print(WARNING, message);
}

void Logger::error(std::string message) {
	print(ERROR, message);
}