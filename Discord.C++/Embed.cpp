#include "Embed.h"

#include "Exceptions.h"
#include "Logger.h"
#include "static.h"

DiscordCPP::Embed::Embed(std::string title, std::string description)
    : _title(std::move(title)),
      _description(std::move(description)) {
}

DiscordCPP::Embed::Embed(const json& data) {
    _title = get_or_else<std::string>(data, "title", "");
    _description = get_or_else<std::string>(data, "description", "");
    _type = get_or_else<std::string>(data, "type", "");
    _url = get_or_else<std::string>(data, "url", "");
    _timestamp = get_or_else<std::string>(data, "timestamp", "");
    _color = get_or_else<int>(data, "color", -1);

    if (has_value(data, "footer")) {
        const json& footer = data.at("footer");

        _footer.text = get_or_else<std::string>(footer, "text", "");
        _footer.icon_url = get_or_else<std::string>(footer, "icon_url", "");
        _footer.proxy_icon_url = get_or_else<std::string>(footer, "proxy_icon_url", "");
    }

    if (has_value(data, "image")) {
        const json& image = data.at("image");

        _image.width = get_or_else<int>(image, "width", 0);
        _image.height = get_or_else<int>(image, "height", 0);
        _image.url = get_or_else<std::string>(image, "url", "");
        _image.proxy_url = get_or_else<std::string>(image, "proxy_url", "");
    }

    if (has_value(data, "video")) {
        const json& video = data.at("video");

        _video.width = get_or_else<int>(video, "width", 0);
        _video.height = get_or_else<int>(video, "height", 0);
        _video.url = get_or_else<std::string>(video, "url", "");
    }

    if (has_value(data, "thumbnail")) {
        const json& thumbnail = data.at("thumbnail");

        _thumbnail.width = get_or_else<int>(thumbnail, "width", 0);
        _thumbnail.height = get_or_else<int>(thumbnail, "height", 0);
        _thumbnail.url = get_or_else<std::string>(thumbnail, "url", "");
        _thumbnail.proxy_url = get_or_else<std::string>(thumbnail, "proxy_url", "");
    }

    if (has_value(data, "author")) {
        const json& author = data.at("author");

        _author.name = get_or_else<std::string>(author, "name", "");
        _author.url = get_or_else<std::string>(author, "url", "");
        _author.icon_url = get_or_else<std::string>(author, "icon_url", "");
        _author.proxy_icon_url = get_or_else<std::string>(author, "proxy_icon_url", "");
    }

    if (has_value(data, "provider")) {
        const json& provider = data.at("provider");

        _provider.name = get_or_else<std::string>(provider, "name", "");
        _provider.url = get_or_else<std::string>(provider, "url", "");
    }

    if (has_value(data, "fields")) {
        for (json field : data.at("fields")) {
            Field f;
            field.at("name").get_to<std::string>(f.name);
            field.at("value").get_to<std::string>(f.value);
            field.at("inline").get_to<bool>(f.is_inline);

            _fields.push_back(f);
        }
    }
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
void DiscordCPP::Embed::add_field(const std::string& name, const std::string& value, bool Inline) {
    if (_fields.size() >= 25) {
        throw SizeError("Embed: Cannot add more than 25 fields.");
    }

    Field field;
    field.name = name;
    field.value = value;
    field.is_inline = Inline;
    _fields.push_back(field);
}

void DiscordCPP::Embed::set_author(const std::string& name, const std::string& url, const std::string& icon_url) {
    _author.name = name;
    _author.url = url;
    _author.icon_url = icon_url;
}

void DiscordCPP::Embed::set_footer(const std::string& text, const std::string& icon_url) {
    _footer.text = text;
    _footer.icon_url = icon_url;
}

void DiscordCPP::Embed::set_image(const std::string& url) {
    _image.url = url;
}

void DiscordCPP::Embed::set_thumbnail(const std::string& url) {
    _thumbnail.url = url;
}

/**	generates json from Embed
        @return	json::value
        @throws	SizeError
*/
json DiscordCPP::Embed::to_json() {
    json ret;

    if (_title.length() > 0)
        ret["title"] = _title;
    if (_description.length() > 0)
        ret["description"] = _description;

    if (_color >= 0)
        ret["color"] = _color;

    for (auto& _field : _fields) {
        ret["fields"].emplace_back((json){
            {"name", _field.name},
            {"value", _field.value},
            {"inline", _field.is_inline}});
    }

    if (_author.name.length() > 0) {
        ret["author"]["name"] = _author.name;
        ret["author"]["url"] = _author.url;
        ret["author"]["icon_url"] = _author.icon_url;
    }

    if (_provider.name.length() > 0) {
        ret["provider"]["name"] = _provider.name;
        ret["provider"]["url"] = _provider.url;
    }

    if (_footer.text.length() > 0) {
        ret["footer"]["text"] = _footer.text;
        ret["footer"]["icon_url"] = _footer.icon_url;
    }

    if (_image.url.length() > 0)
        ret["image"]["url"] = _image.url;

    if (_video.url.length() > 0)
        ret["video"]["url"] = _video.url;

    if (_thumbnail.url.length() > 0)
        ret["thumbnail"]["url"] = _thumbnail.url;

    if (ret.size() == 0) {
        throw SizeError("Cannot create JSON from empty Embed");
    }

    return ret;
}
