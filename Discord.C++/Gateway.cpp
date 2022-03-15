#include "Gateway.h"

#include <time.h>

#include <boost/asio/connect.hpp>
#include <chrono>
#include <exception>

#include "Exceptions.h"
#include "static.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

void DiscordCPP::Gateway::start_heartbeating() {
    static unsigned int heartbeat_task_index = 0;
    unsigned int task_id = heartbeat_task_index++;
    _heartbeat_task = std::thread([this, task_id] {
        Logger::register_thread(std::this_thread::get_id(), "Heartbeat-Thread-" + std::to_string(task_id));
        while (_heartbeat_interval == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        _last_heartbeat_ack = time(0);

        while (_keepalive) {
            if (_last_heartbeat_ack * 1000 + _heartbeat_interval * 2 <
                time(0) * 1000) {
                _log.warning(
                    "Gateway stopped responding. Closing and restarting "
                    "websocket...");
                try {
                    _client->close(websocket::close_reason(websocket::close_code::going_away, "Server not responding"));
                } catch (std::exception& e) {
                    _log.error("Cannot close websocket: " +
                               std::string(e.what()));
                }
            } else {
                json payload = this->get_heartbeat_payload();

                try {
                    this->send(payload).wait();
                    _log.debug("Heartbeat message has been sent");
                } catch (const std::exception& e) {
                    _log.error("Cannot send heartbeat message: " +
                               std::string(e.what()));
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(_heartbeat_interval));
        }
    });
}

/*void DiscordCPP::Gateway::on_websocket_disconnnect(
    const web::websockets::client::websocket_close_status& status,
    const std::string& reason, const std::error_code& error) {
    if (_keepalive == false) {
        return;
    }

    _log.warning("websocket closed with status code " +
                 std::to_string((int)status) + ": " + reason + " (" +
                 std::to_string(error.value()) + ": " + error.message() + ")");

    threadpool->execute([this] {
        try {
            delete _client;
        } catch (const std::exception& e) {
            _log.error("error while deleting old websocket client: " +
                       std::string(e.what()));
        }

        _log.info("trying to reconnect in " +
                  std::to_string((double)_reconnect_timeout / 1000) + "s");
        std::this_thread::sleep_for(std::chrono::milliseconds(_reconnect_timeout));

        if (_reconnect_timeout == 0) {
            _reconnect_timeout = 1000;
        } else if (_reconnect_timeout < 90000) {  // cap timeout to about 15 min
            _reconnect_timeout = (unsigned int)(_reconnect_timeout * 1.5);
        }

        _client->connect(utility::conversions::to_string_t(_url)).wait();
        _log.info("reconnected");
    });
}*/

DiscordCPP::Gateway::Gateway(const std::string& token, const std::shared_ptr<Threadpool>& threadpool) : threadpool(threadpool), io_context(), ssl_context{ssl::context::tlsv12_client} {
    _log = Logger("Discord.Gateway");

    _token = token;

    _heartbeat_interval = 0;

    _reconnect_timeout = 0;

    _keepalive = true;

    _connected = false;

    ssl_context.set_verify_mode(ssl::verify_peer | boost::asio::ssl::verify_fail_if_no_peer_cert);
    load_ssl_certificates(ssl_context);

    _client = std::make_unique<websocket::stream<beast::ssl_stream<tcp::socket>>>(io_context, ssl_context);

    /*_client->set_close_handler([this](websocket_close_status close_status,
                                      utility::string_t reason,
                                      std::error_code error) {
        on_websocket_disconnnect(close_status, utility::conversions::to_utf8string(reason), error);
    });*/
}

DiscordCPP::Gateway::~Gateway() {
    _keepalive = false;
    _heartbeat_task.join();
}

void DiscordCPP::Gateway::set_message_handler(
    const std::function<void(json)>& handler) {
    _message_handler = handler;
}

std::shared_future<void> DiscordCPP::Gateway::connect(const std::string& url) {
    auto connect_future = threadpool->execute([this, url]() {
        _url = url;
        std::string tmp_url = _url;

        // cut protocol
        auto index = tmp_url.find("://");
        if (index != std::string::npos) {
            tmp_url = tmp_url.substr(index + 3, std::string::npos);
        }

        auto port_index = tmp_url.find(":");
        auto query_index = tmp_url.find("?");

        std::string host;
        if (port_index != std::string::npos) {
            host = tmp_url.substr(0, port_index);
        } else {
            host = tmp_url.substr(0, query_index);
        }

        std::string query = "/";
        if (query_index != std::string::npos) {
            query = "/" + tmp_url.substr(query_index, std::string::npos);
        }

        _log.debug("host: " + host + "\t\tquery: " + query);
        _log.info("connecting to websocket: " + _url);

        tcp::resolver resolver{io_context};
        auto results = resolver.resolve(host, "443");

        auto endpoint = net::connect(get_lowest_layer(*_client), results);

        _log.debug("Connected");

        if (!SSL_set_tlsext_host_name(_client->next_layer().native_handle(), host.c_str()))
            throw beast::system_error(
                beast::error_code(
                    static_cast<int>(::ERR_get_error()),
                    net::error::get_ssl_category()),
                "Failed to set SNI Hostname");

        _client->set_option(websocket::stream_base::decorator([](websocket::request_type& req) {
            req.set(http::field::user_agent, "Discord.C++ DiscordBot");
        }));

        _log.debug("Starting handshake");

        _client->next_layer().handshake(ssl::stream_base::client);
        _client->handshake(host + std::string(":") + std::to_string(endpoint.port()), query);

        _log.debug("Handshake complete");

        start_heartbeating();
        _connected = true;
    });

    return threadpool->then(connect_future, [this]() {
        threadpool->execute([this]() {
            _log.debug("Starting message reveiving loop.");
            while (_connected) {
                beast::flat_buffer buffer;
                beast::error_code error_code;
                size_t bytes = _client->read(buffer, error_code);
                _log.debug("Received " + std::to_string(bytes) + " bytes");

                if (error_code) {
                    _log.error("Error while reading message (stopping read loop): " + error_code.message());
                    break;
                }

                std::stringstream message_stream;
                message_stream << beast::make_printable(buffer.data());
                std::string message = message_stream.str();

                threadpool->execute([this, message]() {
                    try {
                        on_websocket_incoming_message(message);
                    } catch (const json::parse_error& e) {
                        _log.error("Error while parsing json: " + std::string(e.what()));
                    } catch (const std::exception& e) {
                        _log.error("Error while handling incoming message: " + std::string(e.what()));
                    }
                });
            }
        });
    });
}

///@throws	ClientException
std::shared_future<void> DiscordCPP::Gateway::send(const json& message) {
    return threadpool->execute([this, message]() {
        if (_connected == false) {
            throw ClientException("Gateway not connected");
        }

        std::string message_string = message.dump();

        _log.debug("sending message: " + message_string);

        beast::error_code error_code;
        _client->write(net::buffer(message_string), error_code);

        if (error_code) {
            throw beast::system_error{error_code};
        }

        _log.debug("Message sent: " + message_string);
    });
}

std::shared_future<void> DiscordCPP::Gateway::close() {
    _keepalive = false;
    _connected = false;

    return threadpool->execute([this]() {
        _client->close(websocket::close_code::normal);
    });
}
