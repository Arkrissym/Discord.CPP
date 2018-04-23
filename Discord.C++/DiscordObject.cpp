#include "DiscordObject.h"
#include "static.h"
#include "Logger.h"

using namespace std;
using namespace web::json;
using namespace web::http;
using namespace web::http::client;
using namespace utility;

DiscordCPP::DiscordObject::DiscordObject() {

}

DiscordCPP::DiscordObject::DiscordObject(string_t token) {
	_token = token;
}

value DiscordCPP::DiscordObject::api_call(string url, method method, value data) {
	http_client c(U(API_URL));
	http_request request(method);

	request.set_request_uri(uri(conversions::to_string_t(url)));
	request.headers().add(U("Authorization"), conversions::to_string_t("Bot " + conversions::to_utf8string(_token)));

	if ((method != methods::GET) && (method != methods::HEAD))
		request.set_body(data);

	Concurrency::task<value> requestTask = c.request(request).then([this, url](http_response response) {
		Logger("discord.object.api_call").debug("api call sent: " + url + ": " + to_string(response.status_code()));

		string response_string = response.extract_utf8string().get();

		//log.debug(response_string);

		value response_data = value::parse(conversions::to_string_t(response_string));
		
		return response_data;
	});

	value ret = value();

	try {
		requestTask.wait();
		ret = requestTask.get();
	}
	catch (const std::exception &e) {
		Logger("discord.object.api_call").error("Error exception: " + string(e.what()));
	}

	return ret;
}