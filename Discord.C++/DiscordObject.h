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
		string_t _token;

		value api_call(string url, method method = methods::GET, value data = value());
	public:
		string id;	//snowflake

		DiscordObject();
		DiscordObject(string_t token);
	};

}