#include "DiscordObject.h"
#include "static.h"
#include "Logger.h"

#include <chrono>
//#include <time.h>
//#include <Windows.h>

using namespace std;
using namespace web::json;
using namespace web::http;
using namespace web::http::client;
using namespace utility;

static vector<shared_ptr<value>> cache;
static bool cache_manager_active = false;

Concurrency::task<void> manage_cache();

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

Concurrency::task<void> manage_cache() {
	return Concurrency::create_task([] {
		while (1) {
			auto it = cache.begin();
			while(it != cache.end()) {
				shared_ptr<value> ptr = *it;

				if ((time(0) - ptr->at(U("time")).as_integer()) > 60) {
					it->reset();
					it = cache.erase(it);

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

/**	@param[in]	url		Relative url
	@param[in]	method	(optional) Method of the http request. Default is GET.
	@param[in]	data	(optional) JSON data to send
*/
value DiscordCPP::DiscordObject::api_call(string url, method method, value data) {
	for (int i = 0; i < cache.size(); i++) {
		if (conversions::to_utf8string(cache[i]->at(U("url")).as_string()) == url) {
			if ((time(0) - cache[i]->at(U("time")).as_integer()) > 60) {
				Logger("discord.object.api_call").debug("found old data");
			}
			else {
				Logger("discord.object.api_call").debug("using cached result for: " + url);
				return value(cache[i]->at(U("data")));
			}
		}
	}
	
	http_client c(U(API_URL));
	http_request request(method);

	request.set_request_uri(uri(conversions::to_string_t(url)));
	request.headers().add(U("Authorization"), conversions::to_string_t("Bot " + conversions::to_utf8string(_token)));

	if ((method != methods::GET) && (method != methods::HEAD))
		request.set_body(data);

	value ret;
	unsigned short code;

	do {
		Concurrency::task<http_response> requestTask = c.request(request).then([this, url](http_response response) {
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
			cache.push_back(make_shared<value>(tmp));

			Logger("discord.object.api_call").debug("caching object");
			
		}

		if (code == 429) {
			Logger("discord.object.api_call").debug("Timeout. Retry after: " + conversions::to_utf8string(requestTask.get().headers()[U("Retry-After")]));

			//Sleep(atoi(conversions::to_utf8string(requestTask.get().headers()[U("Retry-After")]).c_str()));
			this_thread::sleep_for(chrono::milliseconds(atoi(conversions::to_utf8string(requestTask.get().headers()[U("Retry-After")]).c_str())));
		}
	} while (code == 429);

	if (code == 401) {
		throw std::exception("Unauthorized API call");
	}

	return ret;
}