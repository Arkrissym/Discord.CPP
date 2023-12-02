#pragma once
#include <iostream>
#include <map>
#include <string>

#include "static.h"

namespace DiscordCPP {

class DiscordObject {
   private:
    struct http_response {
        unsigned int status_code;
        std::map<std::string, std::string> headers;
        std::string body;
    };
    http_response request_internal(const std::string& url, const std::string& method = "GET", const std::string& data = "", const std::string& content_type = "");

    /// token to authentificate with the discord api
    std::string _token;

    /// the id of the object
    std::string id;

   protected:
    [[nodiscard]] std::string get_token() const { return _token; }
    /// helper function to communicate with the http api
    json api_call(const std::string& url, const std::string& method = "GET", const json& data = json(), const std::string& content_type = "", const bool cache = true);

   public:
    DLL_EXPORT DiscordObject() = default;
    ///	@param[in]	token	Discord token
    DLL_EXPORT explicit DiscordObject(std::string token);
    ///	@param[in]	token	Discord token
    DLL_EXPORT explicit DiscordObject(std::string token, std::string id);

    /// @return the id of the object
    DLL_EXPORT [[nodiscard]] std::string get_id() const { return id; }
};

}  // namespace DiscordCPP
