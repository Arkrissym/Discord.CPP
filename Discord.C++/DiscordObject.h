#pragma once
#include <iostream>
#include <cpprest\json.h>
#include <cpprest\http_client.h>

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace web::http;
	using namespace utility;

	class DiscordObject {
	protected:
		///token to authentificate with the discord api
		string_t _token;

		///helper function to communicate with the http api
		value api_call(string url, method method = methods::GET, value data = value(), string content_type="");
	public:
		///the id of the object
		string id;	//snowflake

		DiscordObject();
		DiscordObject(string_t token);
	};

}