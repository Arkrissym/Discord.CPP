#pragma once
#include <iostream>
#include <cpprest/json.h>
#include <cpprest/http_client.h>

namespace DiscordCPP {

	class DiscordObject {
	protected:
		///token to authentificate with the discord api
		utility::string_t _token;

		///helper function to communicate with the http api
		web::json::value api_call(std::string url, web::http::method method = web::http::methods::GET, web::json::value data = web::json::value(), std::string content_type="", bool cache=true);
	public:
		///the id of the object
		std::string id;	//snowflake

		DiscordObject();
		DiscordObject(utility::string_t token);
	};

}
