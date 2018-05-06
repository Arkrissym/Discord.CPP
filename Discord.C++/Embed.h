#pragma once
#include <iostream>
#include <vector>

namespace DiscordCPP {

	using namespace std;

	class Embed {
	protected:
		string _title;
		string _description;
		string _type;
		string _url;
		string _timestamp;	//ISO8601
		int _color;
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
		Embed();
		~Embed();
	};

}