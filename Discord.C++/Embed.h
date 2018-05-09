#pragma once
#include <iostream>
#include <vector>
#include <cpprest\json.h>

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
		int _color = 0;
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
		__declspec(dllexport) Embed(string title, string description = "");
		__declspec(dllexport) Embed(value data);
		__declspec(dllexport) ~Embed();

		__declspec(dllexport) void set_color(int color);
		__declspec(dllexport) void add_field(string name, string value, bool Inline = true);
		__declspec(dllexport) void set_author(string name, string url="", string icon_url="");
		__declspec(dllexport) void set_footer(string text, string icon_url = "");
		__declspec(dllexport) void set_image(string url);
		__declspec(dllexport) void set_thumbnail(string url);
		__declspec(dllexport) void set_video(string url);

		__declspec(dllexport) value to_json();
	};

}