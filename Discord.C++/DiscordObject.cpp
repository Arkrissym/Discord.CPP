#include "DiscordObject.h"

#include "Exceptions.h"
#include "Logger.h"
#include "static.h"

using namespace web::http;
using namespace web::http::client;
using namespace utility;

using json = nlohmann::json;

static std::vector<std::shared_ptr<json>> _cache;
static bool cache_manager_active = false;

void manage_cache();

DiscordCPP::DiscordObject::DiscordObject(const std::string& token) {
    _token = token;

    if (cache_manager_active == false) {
        cache_manager_active = true;
        manage_cache();
    }
}

void manage_cache() {
    std::thread([] {
        Logger::register_thread(std::this_thread::get_id(), "Cache-Thread");
        Logger log = Logger("discord.object.manage_cache");
        while (1) {
            auto it = _cache.begin();
            while (it != _cache.end()) {
                std::shared_ptr<json> ptr = *it;

                if ((time(0) - ptr->at("time").get<int>()) > 60) {
                    it->reset();
                    it = _cache.erase(it);

                    log.debug("deleted old data");
                } else {
                    it++;
                }
            }

            waitFor(std::chrono::seconds(10)).wait();
        }
    }).detach();
}

/**	@param[in]	url				Relative url
	@param[in]	method			(optional) Method of the http request. Default is GET.
	@param[in]	data			(optional) JSON data to send
	@param[in]	content_type	(optional) the Content-Type of data
	@param[in]	cache			(optional) wether to cache results or not (only GET requests can be cached)
	@return		json::value		API response
	@throws		HTTPError
*/
json DiscordCPP::DiscordObject::api_call(const std::string& url, const method& method, const json& data, const std::string& content_type, const bool cache) {
    if (method == methods::GET && cache == true) {
        for (unsigned int i = 0; i < _cache.size(); i++) {
            if (conversions::to_utf8string(_cache[i]->at("url").get<std::string>()) == url) {
                if ((time(0) - _cache[i]->at("time").get<int>()) > 60) {
                    Logger("discord.object.api_call").debug("found old data");
                } else {
                    Logger("discord.object.api_call").debug("using cached result for: " + url);
                    return json(_cache[i]->at("data"));
                }
            }
        }
    }

    http_client c(U(API_URL));
    http_request request(method);

    request.set_request_uri(uri(conversions::to_string_t(url)));
    request.headers().add(U("Authorization"), conversions::to_string_t("Bot " + conversions::to_utf8string(_token)));
    request.headers().add(U("User-Agent"), conversions::to_string_t("DiscordBot (Discord.C++, " + std::string(VERSION) + ")"));
    if (content_type != "")
        request.headers().set_content_type(conversions::to_string_t(content_type));

    if ((method != methods::GET) && (method != methods::HEAD)) {
        Logger("discord.object.api_call").debug("request body: " + data.dump());
        request.set_body(data.dump());
    }

    json ret;
    unsigned short code;

    do {
        pplx::task<http_response> requestTask = c.request(request).then([url](http_response response) {
            Logger("discord.object.api_call").debug("api call sent: " + std::string(API_URL) + url + ": " + std::to_string(response.status_code()));

            return response;
        });

        try {
            requestTask.wait();
            std::string response = utility::conversions::to_utf8string(requestTask.get().extract_string().get());
            if (response.length() > 0) {
                Logger("discord.object.api_call").debug("response: " + response);
                ret = json::parse(response);
            }
        } catch (const std::exception& e) {
            Logger("discord.object.api_call").error("Error exception: " + std::string(e.what()));
            return json();
        }

        code = requestTask.get().status_code();

        if ((code == 200) && (method == methods::GET)) {
            json tmp = json();
            tmp["url"] = url;
            tmp["time"] = time(0);
            tmp["data"] = ret;

            _cache.push_back(std::make_shared<json>(tmp));

            Logger("discord.object.api_call").debug("caching object");
        }

        if (code == 429) {
            Logger("discord.object.api_call").debug("Rate limit exceeded. Retry after: " + conversions::to_utf8string(requestTask.get().headers()[U("Retry-After")]));

            waitFor(std::chrono::seconds(atoi(conversions::to_utf8string(requestTask.get().headers()[U("Retry-After")]).c_str()))).wait();
        }
    } while (code == 429);

    switch (code) {
        case 400:
            throw HTTPError("Malformed/Invalid API call", code);
        case 401:
            throw HTTPError("Unauthorized API call", code);
        case 403:
            throw HTTPError("Permission denied", code);
        case 500:
            throw HTTPError("Server error", code);
        default:
            if (code >= 300) {
                throw HTTPError("Unknown HTTPError: " + std::to_string(code), code);
            }
    }

    return ret;
}
