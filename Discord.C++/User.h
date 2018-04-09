#pragma once
//#include "snowflake.h"
#include <iostream>
#include <cpprest\json.h>

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class User {
	public:
		string id;	//Snowflake
		string username;
		string discriminator;
		string avatar;
		bool bot = false;
		bool mfa_enabled = false;
		bool verified = false;
		string email;

		User() {};
		User(value data);
	};

}