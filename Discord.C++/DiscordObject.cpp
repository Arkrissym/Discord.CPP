#include "DiscordObject.h"
#include "static.h"
#include "Logger.h"

#include <chrono>
#include <thread>

using namespace std;
using namespace web::json;
using namespace web::http;
using namespace web::http::client;
using namespace utility;

static vector<shared_ptr<value>> _cache;
static bool cache_manager_active = false;

pplx::task<void> manage_cache();

DiscordCPP::DiscordObject::DiscordObject() {

}

///	@param[in]	token	Discord token
DiscordCPP::DiscordObject::DiscordObject(string_t token) {
	_token = token;

	if (cache_manager_active == false) {
		cache_manager_active = true;
		manage_cache();
	}
}

pplx::task<void> manage_cache() {
	return pplx::create_task([] {
		while (1) {
			auto it = _cache.begin();
			while(it != _cache.end()) {
				shared_ptr<value> ptr = *it;

				if ((time(0) - ptr->at(U("time")).as_integer()) > 60) {
					it->reset();
					it = _cache.erase(it);

					Logger("discord.object.manage_cache").debug("deleted old data");
				}
				else {
					it++;
				}
			}

			this_thread::sleep_for(chrono::seconds(10));
		}
	});
}

/**	@param[in]	url				Relative url
	@param[in]	method			(optional) Method of the http request. Default is GET.
	@param[in]	data			(optional) JSON data to send
	@param[in]	content_type	(optional) the Content-Type of data
	@param[in]	cache			(optional) whether
*/
value DiscordCPP::DiscordObject::api_call(string url, method method, value data, string content_type, bool cache) {
	if (method == methods::GET && cache == true) {
		for (int i = 0; i < _cache.size(); i++) {
			if (conversions::to_utf8string(_cache[i]->at(U("url")).as_string()) == url) {
				if ((time(0) - _cache[i]->at(U("time")).as_integer()) > 60) {
					Logger("discord.object.api_call").debug("found old data");
				}
				else {
					Logger("discord.object.api_call").debug("using cached result for: " + url);
					return value(_cache[i]->at(U("data")));
				}
			}
		}
	}

	http_client c(U(API_URL));
	http_request request(method);

	request.set_request_uri(uri(conversions::to_string_t(url)));
	request.headers().add(U("Authorization"), conversions::to_string_t("Bot " + conversions::to_utf8string(_token)));
	request.headers().add(U("User-Agent"), conversions::to_string_t("DiscordBot (github.com/Arkrissym/Discord.CPP, " + string(VERSION) + ")"));
	if (content_type != "")
		request.headers().set_content_type(conversions::to_string_t(content_type));

	if ((method != methods::GET) && (method != methods::HEAD))
		request.set_body(data);

	value ret;
	unsigned short code;

	do {
		pplx::task<http_response> requestTask = c.request(request).then([this, url](http_response response) {
			Logger("discord.object.api_call").debug("api call sent: " + url + ": " + to_string(response.status_code()));

			return response;
		});

		try {
			requestTask.wait();
			ret = value::parse(requestTask.get().extract_string().get());
		}
		catch (const std::exception &e) {
			Logger("discord.object.api_call").error("Error exception: " + string(e.what()));
			return value();
		}

		code = requestTask.get().status_code();

		if ((code == 200) && (method == methods::GET))  {
			value tmp = value();
			tmp[U("url")] = value(conversions::to_string_t(url));
			tmp[U("time")] = value(time(0));
			tmp[U("data")] = ret;

			//cache.push_back(new value(tmp));
			_cache.push_back(make_shared<value>(tmp));

			Logger("discord.object.api_call").debug("caching object");
		}

		if (code == 429) {
			Logger("discord.object.api_call").debug("Rate limit exceeded. Retry after: " + conversions::to_utf8string(requestTask.get().headers()[U("Retry-After")]));

			this_thread::sleep_for(chrono::milliseconds(atoi(conversions::to_utf8string(requestTask.get().headers()[U("Retry-After")]).c_str())));
		}
	} while (code == 429);

	switch (code) {
		case 400: throw runtime_error("Malformed/Invalid API call(400)");
		case 401: throw runtime_error("Unauthorized API call(401)");
		case 403: throw runtime_error("Permission denied(403)");
		case 500: throw runtime_error("Server error(500)");
	}

	return ret;
}
