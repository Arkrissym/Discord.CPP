#pragma once
#include <exception>
#include <iostream>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {
class DiscordException : public std::exception {
   private:
    std::string _what;

   public:
    DLL_EXPORT explicit DiscordException(std::string what);

    DLL_EXPORT [[nodiscard]] const char* what() const noexcept override;
};

class StatusCodeException : public DiscordException {
   private:
    int _error_code;

   public:
    DLL_EXPORT StatusCodeException(std::string what, int error_code);
    DLL_EXPORT [[nodiscard]] int get_error_code() const;
};

class ClientException : public DiscordException {
   public:
    DLL_EXPORT explicit ClientException(std::string what);
};

class SizeError : public DiscordException {
   public:
    DLL_EXPORT explicit SizeError(std::string what);
};

class OpusError : public StatusCodeException {
   public:
    DLL_EXPORT OpusError(std::string what, int error_code);
};

class HTTPError : public StatusCodeException {
   public:
    DLL_EXPORT HTTPError(std::string what, int error_code);
};
}  // namespace DiscordCPP
