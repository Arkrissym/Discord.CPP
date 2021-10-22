#pragma once
#include <string>
#include <vector>

#include "static.h"

namespace DiscordCPP {

class Embed {
   protected:
    std::string _title;
    std::string _description;
    std::string _type;
    std::string _url;
    std::string _timestamp;  //ISO8601
    int _color;
    struct Footer {
        std::string text;
        std::string icon_url;
        std::string proxy_icon_url;
    } _footer;
    struct Image {
        std::string url;
        std::string proxy_url;
        int width;
        int height;
    } _image, _thumbnail;
    struct Video {
        std::string url;
        int width;
        int height;
    } _video;
    struct Provider {
        std::string name;
        std::string url;
    } _provider;
    struct Author {
        std::string name;
        std::string url;
        std::string icon_url;
        std::string proxy_icon_url;
    } _author;
    struct Field {
        std::string name;
        std::string value;
        bool is_inline;
    };
    std::vector<Field> _fields;

   public:
    DLL_EXPORT Embed(const std::string& title = "", const std::string& description = "");
    DLL_EXPORT Embed(const json& data);

    DLL_EXPORT void set_color(int color);
    DLL_EXPORT void add_field(const std::string& name, const std::string& value, const bool Inline = true);
    DLL_EXPORT void set_author(const std::string& name, const std::string& url = "", const std::string& icon_url = "");
    DLL_EXPORT void set_footer(const std::string& text, const std::string& icon_url = "");
    DLL_EXPORT void set_image(const std::string& url);
    DLL_EXPORT void set_thumbnail(const std::string& url);

    DLL_EXPORT json to_json();
};

}  // namespace DiscordCPP
