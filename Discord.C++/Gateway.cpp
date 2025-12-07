#include "Gateway.h"

#include <boost/asio/connect.hpp>
#include <boost/beast/websocket/error.hpp>
#include <chrono>
#include <cstddef>
#include <ctime>
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

        _last_heartbeat_ack = time(nullptr);

        while (_keepalive) {
            if (_connected) {
                if (_last_heartbeat_ack * 1000 + static_cast<time_t>(_heartbeat_interval * 2) < time(nullptr) * 1000) {
                    _log.warning("Gateway stopped responding. Closing and restarting websocket...");
                    try {
                        get_lowest_layer(*_client).close();
                    } catch (std::exception& e) {
                        _log.error("Cannot close websocket: " + std::string(e.what()));
                    }
                } else {
                    json payload = this->get_heartbeat_payload();

                    try {
                        this->send(payload).get();
                        _log.trace("Heartbeat message has been sent");
                    } catch (const std::exception& e) {
                        _log.error("Cannot send heartbeat message: " + std::string(e.what()));
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(_heartbeat_interval));
        }
    });
}

void DiscordCPP::Gateway::on_websocket_disconnnect() {
    _connected = false;
    if (_keepalive == false) {
        _reconnecting = false;
        _reconnect_finished.notify_all();
        return;
    }
    _reconnecting = true;
    _log.info("Websocket connection closed");

    threadpool->execute([this] {
        _log.info("trying to reconnect in " + std::to_string((double)_reconnect_timeout / 1000) + "s");
        std::this_thread::sleep_for(std::chrono::milliseconds(_reconnect_timeout));

        if (_reconnect_timeout == 0) {
            _reconnect_timeout = 1000;
        } else if (_reconnect_timeout < 90000) {  // cap timeout to about 15 min
            _reconnect_timeout = (unsigned int)(_reconnect_timeout * 1.5);
            _resume_url = _url;
        }

        try {
            io_context.restart();
            connect(_resume_url);
            _log.trace("reconnected");
            _reconnect_timeout = 0;
            _last_heartbeat_ack = time(nullptr);
            _reconnecting = false;
            _reconnect_finished.notify_all();
        } catch (const beast::system_error& e) {
            _reconnecting = false;
            _reconnect_finished.notify_all();
            _log.error("Failed to reconnect: " + std::string(e.what()));
            on_websocket_disconnnect();
        }
    });
}

void DiscordCPP::Gateway::on_read(boost::system::error_code error_code, std::size_t bytes) {
    _log.trace("Received " + std::to_string(bytes) + " bytes");

    if (error_code == boost::beast::websocket::error::closed || error_code == boost::beast::errc::operation_canceled || error_code == boost::asio::ssl::error::stream_truncated || bytes <= 0) {
        on_websocket_disconnnect();
        return;
    }

    if (error_code) {
        _log.error("Error while reading message: " + error_code.message());
    } else {
        std::stringstream message_stream;
        message_stream << beast::make_printable(buffer.data());
        std::string message = message_stream.str();

        try {
            on_websocket_incoming_message(message);
        } catch (const std::exception& e) {
            _log.error("Error while handling incoming message: " + std::string(e.what()));
        }
    }

    buffer.clear();
    _client->async_read(buffer, [this](boost::system::error_code ec, std::size_t b) {
        on_read(ec, b);
    });
}

DiscordCPP::Gateway::Gateway(std::string token, const std::shared_ptr<Threadpool>& threadpool)
    : threadpool(threadpool),
      io_context(),
      ssl_context{ssl::context::tlsv12_client},
      _token(std::move(token)),
      _heartbeat_interval(0),
      _keepalive(true),
      _reconnect_timeout(0),
      _connected(false),
      _reconnecting(false),
      _sequence_number(0) {
    _log = Logger("Discord.Gateway");

    ssl_context.set_verify_mode(ssl::verify_peer | boost::asio::ssl::verify_fail_if_no_peer_cert);
    load_ssl_certificates(ssl_context);
}

DiscordCPP::Gateway::~Gateway() {
    _keepalive = false;
    _heartbeat_task.join();
}

void DiscordCPP::Gateway::set_message_handler(const std::function<void(json)>& handler) {
    _message_handler = handler;
}

void DiscordCPP::Gateway::connect(const std::string& url) {
    if (_url.empty()) {
        _url = url;
    }

    std::string tmp_url = url;

    // cut protocol
    auto index = tmp_url.find("://");
    if (index != std::string::npos) {
        tmp_url = tmp_url.substr(index + 3, std::string::npos);
    }

    auto port_index = tmp_url.find(':');
    auto query_index = tmp_url.find('?');

    std::string host;
    std::string port;
    if (port_index != std::string::npos) {
        host = tmp_url.substr(0, port_index);
        port = tmp_url.substr(port_index + 1, query_index - port_index - 1);
    } else {
        host = tmp_url.substr(0, query_index);
        port = "443";
    }

    std::string query = "/";
    if (query_index != std::string::npos) {
        query = "/" + tmp_url.substr(query_index, std::string::npos);
    }

    _log.trace("host: " + host + "\tport: " + port + "\tquery: " + query);
    _log.info("connecting to websocket: " + url);

    tcp::resolver resolver{io_context};
    auto results = resolver.resolve(host, port);

    _client = std::make_unique<websocket::stream<beast::ssl_stream<tcp::socket>>>(io_context, ssl_context);

    auto endpoint = net::connect(get_lowest_layer(*_client), results);

    if (!SSL_set_tlsext_host_name(_client->next_layer().native_handle(), host.c_str())) {
        throw beast::system_error(
            static_cast<int>(::ERR_get_error()),
            net::error::get_ssl_category());
    }

    _client->set_option(websocket::stream_base::decorator([](websocket::request_type& req) {
        req.set(http::field::user_agent, "Discord.C++ DiscordBot");
    }));

    _client->next_layer().handshake(ssl::stream_base::client);
    _client->handshake(host + std::string(":") + std::to_string(endpoint.port()), query);

    if (_heartbeat_task.get_id() == std::thread::id()) {
        start_heartbeating();
    }

    _log.trace("Successfully connected to endpoint: " + endpoint.address().to_string() + ":" + std::to_string(endpoint.port()));
    _connected = true;

    _client->async_read(buffer, [this](boost::system::error_code error_code, std::size_t bytes) {
        on_read(error_code, bytes);
    });

    threadpool->execute([this]() {
        _log.trace("Start io_context");
        io_context.run();
        _log.trace("Stop io_context");
    });
}

///@throws	ClientException
DiscordCPP::SharedFuture<void> DiscordCPP::Gateway::send(const json& message) {
    return threadpool->execute([this, message]() {
        if (_connected == false) {
            throw ClientException("Gateway not connected");
        }

        std::string message_string = message.dump();

        _log.trace("sending message: " + message_string);

        beast::error_code error_code;
        _client->write(net::buffer(message_string), error_code);

        if (error_code) {
            throw beast::system_error{error_code};
        }

        _log.trace("Message sent: " + message_string);
    });
}

void DiscordCPP::Gateway::close() {
    _keepalive = false;

    try {
        get_lowest_layer(*_client).close();
    } catch (const std::exception& e) {
        _log.error("Error while closing websocket: " + std::string(e.what()));
    }

    std::unique_lock<std::mutex> lock(_reconnect_mutex);
    _reconnect_finished.wait(lock, [this]() {
        return !_reconnecting;
    });
}
