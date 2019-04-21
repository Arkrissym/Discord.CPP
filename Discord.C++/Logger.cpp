#include "Logger.h"

using namespace std;

static Loglevel _level = Debug;

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

		std::cout << level_name.c_str() << " - " << _name.c_str() << " - " << message.c_str() << endl;
	}
}

void Logger::debug(std::string message) {
	print(Debug, message);
}

void Logger::info(std::string message) {
	print(Info, message);
}

void Logger::warning(std::string message) {
	print(Warning, message);
}

void Logger::error(std::string message) {
	print(Error, message);
}