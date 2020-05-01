#include "Logger.h"

using namespace std;

static Loglevel _level = Debug;

Logger::Logger() {

}

Logger::~Logger() {

}

Logger::Logger(const std::string& name) {
	_name = name;
}

Logger::Logger(const Logger& old) {
	_name = old._name;
}

void Logger::set_log_level(const Loglevel& level) {
	_level = level;
}

void Logger::print(const Loglevel level, const std::string& message) {
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

void Logger::debug(const std::string& message) {
	print(Debug, message);
}

void Logger::info(const std::string& message) {
	print(Info, message);
}

void Logger::warning(const std::string& message) {
	print(Warning, message);
}

void Logger::error(const std::string& message) {
	print(Error, message);
}