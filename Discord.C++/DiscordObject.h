#pragma once
#include <cpprest/http_client.h>
#include <cpprest/json.h>

#include <iostream>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

class DiscordObject {
   protected:
    ///token to authentificate with the discord api
    utility::string_t _token;

    ///helper function to communicate with the http api
    web::json::value api_call(const std::string& url, const web::http::method& method = web::http::methods::GET, const web::json::value& data = web::json::value(), const std::string& content_type = "", const bool cache = true);

   public:
    ///the id of the object
    std::string id;  //snowflake

    DLL_EXPORT DiscordObject(){};
    ///	@param[in]	token	Discord token
    DLL_EXPORT DiscordObject(const utility::string_t& token);
};

}  // namespace DiscordCPP
