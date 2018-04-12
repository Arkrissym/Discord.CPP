#include "Message.h"

using namespace std;
using namespace web::json;
using namespace utility;

DiscordCPP::Message::Message(value data) {
	id = conversions::to_utf8string(data.at(U("id")).as_string());
	
	channel_id = conversions::to_utf8string(data.at(U("channel_id")).as_string());
	
	author = User(data.at(U("author")));

	content = conversions::to_utf8string(data.at(U("content")).as_string());


}

DiscordCPP::Message::Message() {

}
