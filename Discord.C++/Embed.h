#pragma once
#include <iostream>
#include <vector>
#include <cpprest/json.h>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;

	class Embed {
	protected:
		string _title;
		string _description;
		string _type;
		string _url;
		string _timestamp;	//ISO8601
		int _color = -1;
		struct Footer {
			string text;
			string icon_url;
			string proxy_icon_url;
		} _footer;
		struct Image {
			string url;
			string proxy_url;
			int width;
			int height;
		} _image, _thumbnail;
		struct Video {
			string url;
			int width;
			int height;
		} _video;
		struct Provider {
			string name;
			string url;
		} _provider;
		struct Author {
			string name;
			string url;
			string icon_url;
			string proxy_icon_url;
		} _author;
		struct Field {
			string name;
			string value;
			bool is_inline;
		};
		vector<Field> _fields;
	public:
		DLL_EXPORT Embed(const string& title = "", const string& description = "");
		DLL_EXPORT Embed(value data);
		DLL_EXPORT ~Embed();

		DLL_EXPORT void set_color(int color);
		DLL_EXPORT void add_field(const string& name, const string& value, const bool Inline = true);
		DLL_EXPORT void set_author(const string& name, const string& url = "", const string& icon_url = "");
		DLL_EXPORT void set_footer(const string& text, const string& icon_url = "");
		DLL_EXPORT void set_image(const string& url);
		DLL_EXPORT void set_thumbnail(const string& url);

		DLL_EXPORT value to_json();
	};

}
