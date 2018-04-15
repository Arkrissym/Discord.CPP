#pragma once
#include "Logger.h"
#include "User.h"

#include <iostream>
#include <cpprest\json.h>

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class Member : public User {
	public:
		string nick;
		//vector<Role> roles;
		string joined_at;	//ISO8601 timestamp
		bool deaf;
		bool mute;

		__declspec(dllexport) Member(value data, string_t token);
		__declspec(dllexport) Member();
		__declspec(dllexport) ~Member();

	};

}