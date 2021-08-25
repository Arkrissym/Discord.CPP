#include "Exceptions.h"

DiscordCPP::DiscordException::DiscordException(const std::string& what) {
    _what = what;
}

DiscordCPP::DiscordException::~DiscordException() {
}

const char* DiscordCPP::DiscordException::what() const noexcept {
    return _what.c_str();
}

DiscordCPP::StatusCodeException::StatusCodeException(const std::string& what, int error_code) : DiscordException(what) {
    _error_code = error_code;
}

DiscordCPP::StatusCodeException::~StatusCodeException() {
}

///@return	error code of this exception
int DiscordCPP::StatusCodeException::get_error_code() const {
    return _error_code;
}

DiscordCPP::ClientException::ClientException(const std::string& what) : DiscordException(what) {
}

DiscordCPP::SizeError::SizeError(const std::string& what) : DiscordException(what) {
}

DiscordCPP::OpusError::OpusError(const std::string& what, int error_code) : StatusCodeException(what, error_code) {
}

DiscordCPP::HTTPError::HTTPError(const std::string& what, int error_code) : StatusCodeException(what, error_code) {
}
