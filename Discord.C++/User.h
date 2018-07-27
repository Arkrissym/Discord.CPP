#pragma once
#include "DiscordObject.h"
//#include "Message.h"
//#include "Embed.h"
//#include "DMChannel.h"

#include <iostream>

#include "Logger.h"

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class Message;
	class Embed;
	class DMChannel;

	class User : public DiscordCPP::DiscordObject {
	public:
		///ther user's name
		string username;
		///the user's discord-tag
		string discriminator;
		///the user's avatar hash
		string avatar;
		///true, if the user is a bot
		bool bot = false;
		///true, if the user has enabled two factor authentification
		bool mfa_enabled = false;
		///true, if the user's email has been verified
		bool verified = false;
		///the user's email
		string email;

		__declspec(dllexport) User();
		__declspec(dllexport) User(value data, string_t token);
		__declspec(dllexport) User(string id, string_t token);
		__declspec(dllexport) ~User();
		///get/create the DMChannel for this user
		__declspec(dllexport) DMChannel get_dmchannel();
		///send a message to this user
		__declspec(dllexport) Message send(string content, bool tts = false);
		///send an embed to this user
		__declspec(dllexport) Message send(Embed embed);

		///@return Username as std::string
		__declspec(dllexport) virtual operator string() { return username; };
	};

}