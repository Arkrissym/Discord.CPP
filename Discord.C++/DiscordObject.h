#pragma once
#include <cpprest/http_client.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "static.h"

namespace DiscordCPP {

class DiscordObject {
   protected:
    ///token to authentificate with the discord api
    std::string _token;

    ///helper function to communicate with the http api
    json api_call(const std::string& url, const web::http::method& method = web::http::methods::GET, const json& data = json(), const std::string& content_type = "", const bool cache = true);

   public:
    ///the id of the object
    std::string id;  //snowflake

    DLL_EXPORT DiscordObject(){};
    ///	@param[in]	token	Discord token
    DLL_EXPORT DiscordObject(const std::string& token);
};

}  // namespace DiscordCPP
