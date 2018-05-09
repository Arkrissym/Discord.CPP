#include "Embed.h"
#include "static.h"
#include "Logger.h"

using namespace std;
using namespace web::json;
using namespace utility;

DiscordCPP::Embed::Embed(string title, string description) {
	_title = title;
	_description = description;
}

DiscordCPP::Embed::Embed(value data) {
	//Logger("discord.embed").debug(conversions::to_utf8string(data.serialize()));

	if (is_valid_field("title")) {
		_title = conversions::to_utf8string(data.at(U("title")).as_string());
	}
	if (is_valid_field("description")) {
		_description = conversions::to_utf8string(data.at(U("description")).as_string());
	}
	if (is_valid_field("type")) {
		_type = conversions::to_utf8string(data.at(U("type")).as_string());
	}
	if (is_valid_field("url")) {
		_url = conversions::to_utf8string(data.at(U("url")).as_string());
	}
	if (is_valid_field("timestamp")) {
		_timestamp = conversions::to_utf8string(data.at(U("timestamp")).as_string());
	}
	if (is_valid_field("color")) {
		_color = data.at(U("color")).as_integer();
	}
	if (is_valid_field("footer")) {
		_footer.text = conversions::to_utf8string(data[U("footer")][U("text")].as_string());
		_footer.icon_url = conversions::to_utf8string(data[U("footer")][U("icon_url")].as_string());
		_footer.proxy_icon_url = conversions::to_utf8string(data[U("footer")][U("proxy_icon_url")].as_string());
	}
	//image
	//video
	//thumbnail
	//author
	//provider
	if (is_valid_field("fields")) {
		web::json::array tmp = data.at(U("fields")).as_array();
		for (int i = 0; i < tmp.size(); i++) {
			Field f;
			f.name = conversions::to_utf8string(tmp[i].at(U("name")).as_string());
			f.value = conversions::to_utf8string(tmp[i].at(U("value")).as_string());
			f.is_inline = tmp[i].at(U("inline")).as_bool();

			_fields.push_back(f);
		}
	}
}

DiscordCPP::Embed::~Embed() {

}

void DiscordCPP::Embed::set_color(int color) {
	_color = color;
}

void DiscordCPP::Embed::add_field(string name, string value, bool Inline) {
	if (_fields.size() >= 25) {
		throw exception("Embed: Cannot add more than 25 fields.");
	}

	Field field;
	field.name = name;
	field.value = value;
	field.is_inline = Inline;
	_fields.push_back(field);
}

void DiscordCPP::Embed::set_author(string name, string url, string icon_url) {
	_author.name = name;
	_author.url = url;
	_author.icon_url = icon_url;
	//_author.proxy_icon_url=
}

void DiscordCPP::Embed::set_footer(string text, string icon_url) {
	Footer footer;
	footer.text = text;
	footer.icon_url = icon_url;
	//footer.proxy_icon_url=
	_footer = footer;
}

void DiscordCPP::Embed::set_image(string url) {

}

void DiscordCPP::Embed::set_thumbnail(string url) {

}

void DiscordCPP::Embed::set_video(string url) {

}

value DiscordCPP::Embed::to_json() {
	value ret;

	ret[U("title")] = value(conversions::to_string_t(_title));
	ret[U("description")] = value(conversions::to_string_t(_description));
	ret[U("color")] = value(_color);

	for (int i = 0; i < _fields.size(); i++) {
		value f;
		f[U("name")] = value(conversions::to_string_t(_fields[i].name));
		f[U("value")] = value(conversions::to_string_t(_fields[i].value));
		f[U("inline")] = value(_fields[i].is_inline);
		ret[U("fields")][i] = f;
	}

	ret[U("author")][U("name")] = value(conversions::to_string_t(_author.name));
	ret[U("author")][U("url")] = value(conversions::to_string_t(_author.url));
	ret[U("author")][U("icon_url")] = value(conversions::to_string_t(_author.icon_url));
	//ret[U("author")][U("proxy_icon_url")] = 

	return ret;
}
