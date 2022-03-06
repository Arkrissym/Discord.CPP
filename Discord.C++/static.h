#pragma once
#define VERSION "0.11.0"

#define GATEWAY_URL "wss://gateway.discord.gg?v=9&encoding=json&compress=zlib-stream"
#define API_PREFIX "/api/v9"
#define DISCORD_HOST "discord.com"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include <boost/asio/ssl/context.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

DLL_EXPORT void hexchar(unsigned char c, unsigned char& hex1,
                        unsigned char& hex2);
DLL_EXPORT std::string urlencode(std::string s);

DLL_EXPORT void load_ssl_certificates(boost::asio::ssl::context& ssl_context);

DLL_EXPORT inline bool has_value(const json& j, const std::string& key) {
    return j.count(key) > 0 && !j.at(key).is_null();
}

template <class T>
DLL_EXPORT T get_or_else(const json& j, const std::string& key, const T& _default) {
    if (j.count(key) == 0 || j.at(key).is_null())
        return _default;
    else
        return j.at(key).get<T>();
}
