#pragma once
#define VERSION "0.11.0"

#define GATEWAY_URL \
    "wss://gateway.discord.gg?v=9&encoding=json&compress=zlib-stream"
#define API_URL "https://discord.com/api/v9"

#define is_valid_field(name) \
    (data.has_field(U(name))) && (!data.at(U(name)).is_null())

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include <chrono>
#include <future>

DLL_EXPORT std::shared_future<void> waitFor(const std::chrono::microseconds time);

DLL_EXPORT void hexchar(unsigned char c, unsigned char& hex1,
                        unsigned char& hex2);
DLL_EXPORT std::string urlencode(std::string s);
