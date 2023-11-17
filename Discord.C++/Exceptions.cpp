#include "Exceptions.h"

#include <utility>

DiscordCPP::DiscordException::DiscordException(std::string what)
    : _what(std::move(what)) {
}

const char* DiscordCPP::DiscordException::what() const noexcept {
    return _what.c_str();
}

DiscordCPP::StatusCodeException::StatusCodeException(std::string what, int error_code)
    : DiscordException(std::move(what)),
      _error_code(error_code) {
}

///@return	error code of this exception
int DiscordCPP::StatusCodeException::get_error_code() const {
    return _error_code;
}

DiscordCPP::ClientException::ClientException(std::string what)
    : DiscordException(std::move(what)) {
}

DiscordCPP::SizeError::SizeError(std::string what)
    : DiscordException(std::move(what)) {
}

DiscordCPP::OpusError::OpusError(std::string what, int error_code)
    : StatusCodeException(std::move(what), error_code) {
}

DiscordCPP::HTTPError::HTTPError(std::string what, int error_code)
    : StatusCodeException(std::move(what), error_code) {
}
