#include "DiscordObject.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <utility>

#include "Exceptions.h"
#include "Logger.h"
#include "static.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

static std::vector<std::shared_ptr<json>> _cache;
static bool cache_manager_active = false;

void manage_cache();

DiscordCPP::DiscordObject::DiscordObject(std::string token)
    : _token(std::move(token)) {
    if (cache_manager_active == false) {
        cache_manager_active = true;
        manage_cache();
    }
}

DiscordCPP::DiscordObject::DiscordObject(std::string token, std::string id)
    : _token(std::move(token)),
      id(std::move(id)) {
}

void manage_cache() {
    std::thread([] {
        Logger::register_thread(std::this_thread::get_id(), "Cache-Thread");
        Logger log = Logger("discord.object.manage_cache");
        while (true) {
            auto it = _cache.begin();
            while (it != _cache.end()) {
                std::shared_ptr<json> ptr = *it;

                if ((time(nullptr) - ptr->at("time").get<int>()) > 60) {
                    it->reset();
                    it = _cache.erase(it);

                    log.debug("deleted old data");
                } else {
                    it++;
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(10));
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
json DiscordCPP::DiscordObject::api_call(const std::string& url, const std::string& method, const json& data, const std::string& content_type, const bool cache) {
    if (method == "GET" && cache == true) {
        for (auto& i : _cache) {
            if (i->at("url").get<std::string>() == url) {
                Logger("discord.object.api_call").debug("using cached result for: " + url);
                return json(i->at("data"));
            }
        }
    }

    json ret;
    unsigned short code = 429;

    while (code == 429) {
        http_response response;
        try {
            response = request_internal(API_PREFIX + url, method, data.size() > 0 ? data.dump() : "", content_type);

            if (response.body.length() > 0) {
                ret = json::parse(response.body);
            }
        } catch (const std::exception& e) {
            Logger("discord.object.api_call").error("Error exception: " + std::string(e.what()));
            throw ClientException("HTTP request failed because: " + std::string(e.what()));
        }

        code = response.status_code;

        if (code == 429) {
            Logger("discord.object.api_call").debug("Rate limit exceeded. Retry after: " + response.headers.at("Retry-After"));

            std::this_thread::sleep_for(std::chrono::seconds(atoi(response.headers.at("Retry-After").c_str())));
        }
    }

    switch (code) {
        case 200:
            if (method == "GET") {
                json tmp = json();
                tmp["url"] = url;
                tmp["time"] = time(nullptr);
                tmp["data"] = ret;

                _cache.push_back(std::make_shared<json>(tmp));

                Logger("discord.object.api_call").debug("caching object");
            }
            break;
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

DiscordCPP::DiscordObject::http_response DiscordCPP::DiscordObject::request_internal(const std::string& target, const std::string& method, const std::string& data, const std::string& content_type) {
    Logger("discord.object.request_internal").debug("sending message " + data + " via " + method + " to https://" + DISCORD_HOST + target);

    ssl::context ssl_context(ssl::context::tlsv13);
    ssl_context.set_verify_mode(ssl::verify_peer | boost::asio::ssl::verify_fail_if_no_peer_cert);
    load_ssl_certificates(ssl_context);

    net::io_context io_context;
    tcp::resolver resolver(io_context);

    auto const results = resolver.resolve(DISCORD_HOST, "https");
    ssl::stream<tcp::socket> stream(io_context, ssl_context);
    boost::asio::connect(stream.lowest_layer(), results);

    if (!SSL_set_tlsext_host_name(stream.native_handle(), DISCORD_HOST))
        throw beast::system_error(
            beast::error_code(
                static_cast<int>(::ERR_get_error()),
                net::error::get_ssl_category()),
            "Failed to set SNI Hostname");

    stream.handshake(ssl::stream_base::client);

    http::request<http::string_body> request{http::string_to_verb(method), target, 11};
    request.set(http::field::host, DISCORD_HOST);
    request.set(http::field::user_agent, "Discord.C++ DiscordBot");
    request.set(http::field::authorization, "Bot " + _token);
    request.set(http::field::connection, "Close");
    if (data.length() > 0) {
        Logger("discord.object.request_internal").debug("setting message body \"" + data + "\" with content type \"" + content_type + "\" and content length \"" + std::to_string(data.length()) + "\"");
        request.set(http::field::content_type, content_type);
        request.set(http::field::content_length, std::to_string(data.length()));
        request.body() = data;
    }

    http::write(stream, request);

    beast::flat_buffer buffer;
    http::response<http::string_body> response;
    http::read(stream, buffer, response);

    Logger("discord.object.request_internal").debug("response: " + std::to_string(response.result_int()) + ": " + response.body());

    beast::error_code error_code;
    stream.shutdown(error_code);

    // according to the documentation not_connected can happen even if no real error appeared.
    // eof is intended behaviour when closing ssl connections
    if (error_code && error_code != beast::errc::not_connected && error_code != boost::asio::error::eof && error_code != boost::asio::ssl::error::stream_truncated) {
        throw beast::system_error{error_code};
    }

    std::map<std::string, std::string> response_headers;
    for (const auto& it : response.base()) {
        response_headers.emplace(it.name_string(), it.value());
    }

    return http_response{response.result_int(), response_headers, response.body()};
}
