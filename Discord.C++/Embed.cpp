#include "Embed.h"
#include "static.h"
#include "Logger.h"
#include "Exceptions.h"

#include <string>

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
		if ((data[U("footer")].has_field(U("icon_url"))) && (!data[U("footer")].at(U("icon_url")).is_null()))
			_footer.icon_url = conversions::to_utf8string(data[U("footer")][U("icon_url")].as_string());
		if ((data[U("footer")].has_field(U("proxy_icon_url"))) && (!data[U("footer")].at(U("proxy_icon_url")).is_null()))
			_footer.proxy_icon_url = conversions::to_utf8string(data[U("footer")][U("proxy_icon_url")].as_string());
	}
	if (is_valid_field("image")) {
		_image.width = data[U("image")][U("width")].as_integer();
		_image.height = data[U("image")][U("height")].as_integer();
		_image.url = conversions::to_utf8string(data[U("image")][U("url")].as_string());
		_image.proxy_url = conversions::to_utf8string(data[U("image")][U("proxy_url")].as_string());
	}
	if (is_valid_field("video")) {
		_video.width = data[U("video")][U("width")].as_integer();
		_video.height = data[U("video")][U("height")].as_integer();
		_video.url = conversions::to_utf8string(data[U("video")][U("url")].as_string());
	}
	if (is_valid_field("thumbnail")) {
		_thumbnail.width = data[U("thumbnail")][U("width")].as_integer();
		_thumbnail.height = data[U("thumbnail")][U("height")].as_integer();
		_thumbnail.url = conversions::to_utf8string(data[U("thumbnail")][U("url")].as_string());
		_thumbnail.proxy_url = conversions::to_utf8string(data[U("thumbnail")][U("proxy_url")].as_string());
	}
	if (is_valid_field("author")) {
		_author.name = conversions::to_utf8string(data[U("author")][U("name")].as_string());
		if ((data[U("author")].has_field(U("url"))) && (!data[U("author")].at(U("url")).is_null()))
			_author.url = conversions::to_utf8string(data[U("author")][U("url")].as_string());
		if ((data[U("author")].has_field(U("icon_url"))) && (!data[U("author")].at(U("icon_url")).is_null()))
			_author.icon_url = conversions::to_utf8string(data[U("author")][U("icon_url")].as_string());
		if ((data[U("author")].has_field(U("proxy_icon_url"))) && (!data[U("author")].at(U("proxy_icon_url")).is_null()))
			_author.proxy_icon_url = conversions::to_utf8string(data[U("author")][U("proxy_icon_url")].as_string());
	}
	if (is_valid_field("provider")) {
		_provider.name = conversions::to_utf8string(data[U("provider")][U("name")].as_string());
		_provider.url = conversions::to_utf8string(data[U("provider")][U("url")].as_string());
	}
	if (is_valid_field("fields")) {
		web::json::array tmp = data.at(U("fields")).as_array();
		for (unsigned int i = 0; i < tmp.size(); i++) {
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

/**	add a field to the Embed
	@param[in]	name	the name of the field
	@param[in]	value	the value of the field
	@param[in]	Inline	(optional) wether the field shall be displayed inline or not(default is true)
	@throws	SizeError
*/
void DiscordCPP::Embed::add_field(string name, string value, bool Inline) {
	if (_fields.size() >= 25) {
		throw SizeError("Embed: Cannot add more than 25 fields.");
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
}

void DiscordCPP::Embed::set_footer(string text, string icon_url) {
	_footer.text = text;
	_footer.icon_url = icon_url;
}

void DiscordCPP::Embed::set_image(string url) {
	_image.url = url;
}

void DiscordCPP::Embed::set_thumbnail(string url) {
	_thumbnail.url = url;
}

/**	generates json from Embed
	@return	json::value
	@throws	SizeError
*/
value DiscordCPP::Embed::to_json() {
	value ret;

	if (_title.length() > 0)
		ret[U("title")] = value(conversions::to_string_t(_title));
	if (_description.length() > 0)
		ret[U("description")] = value(conversions::to_string_t(_description));

	if (_color >= 0)
		ret[U("color")] = value(_color);

	for (unsigned int i = 0; i < _fields.size(); i++) {
		value f;
		f[U("name")] = value(conversions::to_string_t(_fields[i].name));
		f[U("value")] = value(conversions::to_string_t(_fields[i].value));
		f[U("inline")] = value(_fields[i].is_inline);
		ret[U("fields")][i] = f;
	}

	if (_author.name.length() > 0) {
		ret[U("author")][U("name")] = value(conversions::to_string_t(_author.name));
		ret[U("author")][U("url")] = value(conversions::to_string_t(_author.url));
		ret[U("author")][U("icon_url")] = value(conversions::to_string_t(_author.icon_url));
	}

	if (_provider.name.length() > 0) {
		ret[U("provider")][U("name")] = value(conversions::to_string_t(_provider.name));
		ret[U("provider")][U("url")] = value(conversions::to_string_t(_provider.url));
	}

	if (_footer.text.length() > 0) {
		ret[U("footer")][U("text")] = value(conversions::to_string_t(_footer.text));
		ret[U("footer")][U("icon_url")] = value(conversions::to_string_t(_footer.icon_url));
	}

	if (_image.url.length() > 0)
		ret[U("image")][U("url")] = value(conversions::to_string_t(_image.url));

	if (_video.url.length() > 0)
		ret[U("video")][U("url")] = value(conversions::to_string_t(_video.url));

	if (_thumbnail.url.length() > 0)
		ret[U("thumbnail")][U("url")] = value(conversions::to_string_t(_thumbnail.url));

	if (ret.size() == 0) {
		throw SizeError("Cannot create JSON from empty Embed");
	}

	//Logger("discord.embed").debug(conversions::to_utf8string(ret.serialize()));

	return ret;
}
