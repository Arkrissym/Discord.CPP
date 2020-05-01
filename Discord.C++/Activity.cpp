#include "Activity.h"

using namespace std;
using namespace web::json;
using namespace utility;

DiscordCPP::Activity::Activity() {
	type = ActivityTypes::NoActivity;
}

DiscordCPP::Activity::Activity(const string& name, const int type, const string& url) {
	this->name = name;
	this->type = type;
	this->url = url;
}

DiscordCPP::Activity::~Activity() {

}

value DiscordCPP::Activity::to_json() {
	value ret;

	ret[U("name")] = value(conversions::to_string_t(name));
	ret[U("type")] = value(type);
	if (type == ActivityTypes::Streaming)
		ret[U("url")] = value(conversions::to_string_t(url));

	return ret;
}
