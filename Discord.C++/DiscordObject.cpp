#include "DiscordObject.h"
#include "static.h"
#include "Logger.h"
#include "Exceptions.h"

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
DiscordCPP::DiscordObject::DiscordObject(const utility::string_t& token) {
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
			while (it != _cache.end()) {
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

			waitFor(chrono::seconds(10)).wait();
		}
	});
}

/**	@param[in]	url				Relative url
	@param[in]	method			(optional) Method of the http request. Default is GET.
	@param[in]	data			(optional) JSON data to send
	@param[in]	content_type	(optional) the Content-Type of data
	@param[in]	cache			(optional) wether to cache results or not (only GET requests can be cached)
	@return		json::value		API response
	@throws		HTTPError
*/
value DiscordCPP::DiscordObject::api_call(const string& url, const method& method, const value& data, const string& content_type, const bool cache) {
	if (method == methods::GET && cache == true) {
		for (unsigned int i = 0; i < _cache.size(); i++) {
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
	request.headers().add(U("User-Agent"), conversions::to_string_t("DiscordBot (Discord.C++, " + string(VERSION) + ")"));
	if (content_type != "")
		request.headers().set_content_type(conversions::to_string_t(content_type));

	if ((method != methods::GET) && (method != methods::HEAD))
		request.set_body(data);

	value ret;
	unsigned short code;

	do {
		pplx::task<http_response> requestTask = c.request(request).then([this, url](http_response response) {
			Logger("discord.object.api_call").debug("api call sent: " + string(API_URL) + url + ": " + to_string(response.status_code()));

			return response;
		});

		try {
			requestTask.wait();
			ret = value::parse(requestTask.get().extract_string().get());
		}
		catch (const std::exception& e) {
			Logger("discord.object.api_call").error("Error exception: " + string(e.what()));
			return value();
		}

		code = requestTask.get().status_code();

		if ((code == 200) && (method == methods::GET)) {
			value tmp = value();
			tmp[U("url")] = value(conversions::to_string_t(url));
			tmp[U("time")] = value(time(0));
			tmp[U("data")] = ret;

			_cache.push_back(make_shared<value>(tmp));

			Logger("discord.object.api_call").debug("caching object");
		}

		if (code == 429) {
			Logger("discord.object.api_call").debug("Rate limit exceeded. Retry after: " + conversions::to_utf8string(requestTask.get().headers()[U("Retry-After")]));

			waitFor(chrono::milliseconds(atoi(conversions::to_utf8string(requestTask.get().headers()[U("Retry-After")]).c_str()))).wait();
		}
	} while (code == 429);

	switch (code) {
	case 400: throw HTTPError("Malformed/Invalid API call", code);
	case 401: throw HTTPError("Unauthorized API call", code);
	case 403: throw HTTPError("Permission denied", code);
	case 500: throw HTTPError("Server error", code);
	default:
		if (code >= 300) {
			throw HTTPError("Unknown HTTPError: " + to_string(code), code);
		}
	}

	return ret;
}
