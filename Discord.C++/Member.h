#pragma once
//#include "Logger.h"
#include "User.h"

#include <iostream>
#include <cpprest\json.h>

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class Member : public User {
	public:
		///the member's nickname
		string nick;
		//vector<Role> roles;
		///when the member joined the server
		string joined_at;	//ISO8601 timestamp
		///true, if the member is defeaned
		bool deaf = false;
		///true, if the member is muted
		bool mute = false;

		__declspec(dllexport) Member(value data, string_t token);
		__declspec(dllexport) Member();
		__declspec(dllexport) ~Member();

	};

}