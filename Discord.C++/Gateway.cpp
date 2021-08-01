#include "Gateway.h"

#include <time.h>

#include <chrono>
#include <exception>

#include "Exceptions.h"
#include "static.h"

using namespace web::websockets::client;
using namespace web::json;
using namespace utility::conversions;

std::string DiscordCPP::Gateway::decompress_message(
    const std::string& message) {
    zs.next_in = (unsigned char*)message.data();
    zs.avail_in = (uInt)message.size();
    zs.total_out = 0;

    int ret;
    char buf[8192];
    std::string out;

    do {
        zs.next_out = (unsigned char*)buf;
        zs.avail_out = sizeof(buf);

        ret = inflate(&zs, Z_NO_FLUSH);

        if (out.size() < zs.total_out) {
            out.append(buf, zs.total_out - out.size());
        }
    } while (ret == Z_OK && zs.avail_in > 0);

    if (ret != Z_OK) {
        throw ClientException("Failed to decompress message");
    }

    return out;
}

void DiscordCPP::Gateway::start_heartbeating() {
    _heartbeat_task = pplx::create_task([this] {
        while (_heartbeat_interval == 0)
            waitFor(std::chrono::milliseconds(50)).wait();

        _last_heartbeat_ack = time(0);

        while (_keepalive) {
            if (_last_heartbeat_ack * 1000 + _heartbeat_interval * 2 <
                time(0) * 1000) {
                _log.warning(
                    "Gateway stopped responding. Closing and restarting "
                    "websocket...");
                try {
                    _client
                        ->close(websocket_close_status::going_away,
                                U("Server not responding"))
                        .wait();
                } catch (std::exception& e) {
                    _log.error("Cannot close websocket: " +
                               std::string(e.what()));
                }
            } else {
                value payload = this->get_heartbeat_payload();

                try {
                    this->send(payload)
                        .then([this] {
                            _log.debug("Heartbeat message has been sent");
                        })
                        .wait();
                } catch (websocket_exception& e) {
                    _log.error("Cannot send heartbeat message: " +
                               std::string(e.what()) + " (" +
                               std::to_string(e.error_code().value()) + ": " +
                               e.error_code().message());
                } catch (const std::exception& e) {
                    _log.error("Cannot send heartbeat message: " +
                               std::string(e.what()));
                }
            }

            waitFor(std::chrono::milliseconds(_heartbeat_interval)).wait();
        }
    });
}

void DiscordCPP::Gateway::on_websocket_disconnnect(
    const web::websockets::client::websocket_close_status& status,
    const std::string& reason, const std::error_code& error) {
    if (_keepalive == false) {
        return;
    }

    _log.warning("websocket closed with status code " +
                 std::to_string((int)status) + ": " + reason + " (" +
                 std::to_string(error.value()) + ": " + error.message() + ")");

    pplx::create_task([this] {
        try {
            delete _client;
        } catch (const std::exception& e) {
            _log.error("error while deleting old websocket client: " +
                       std::string(e.what()));
        }

        _log.info("trying to reconnect in " +
                  std::to_string((double)_reconnect_timeout / 1000) + "s");
        waitFor(std::chrono::milliseconds(_reconnect_timeout)).wait();

        if (_reconnect_timeout == 0) {
            _reconnect_timeout = 1000;
        } else if (_reconnect_timeout < 90000) {  // cap timeout to about 15 min
            _reconnect_timeout = (unsigned int)(_reconnect_timeout * 1.5);
        }

        _client->connect(to_string_t(_url)).wait();
        _log.info("reconnected");
    });
}

DiscordCPP::Gateway::Gateway(const std::string& token) : threadpool() {
    _log = Logger("Discord.Gateway");

    _token = token;

    _heartbeat_interval = 0;

    _reconnect_timeout = 0;

    _keepalive = true;

    _connected = false;

    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.avail_in = 0;
    zs.next_in = Z_NULL;

    if (inflateInit(&zs) != Z_OK) {
        throw ClientException("Failed to initialize zlib");
    }

    _client = new websocket_callback_client();

    _client->set_message_handler([this](websocket_incoming_message msg) {
        std::string message;

        if (msg.message_type() == websocket_message_type::binary_message) {
            concurrency::streams::container_buffer<std::string> buf;
            msg.body().read_to_end(buf).wait();
            message = decompress_message(buf.collection());
        } else {
            message = msg.extract_string().get();
        }

        _log.debug("received message: " + message);

        value payload = value::parse(message);

        std::future<std::string> f = threadpool.execute([this, payload]() {
            on_websocket_incoming_message(payload);
            return std::string("asdf");
        });
        //f.wait();
        /*auto f2 = threadpool.execute([this, &f]() {
            _log.debug(f.get());
        });
        f2.wait();*/
        auto f2 = threadpool.then(f, [this](std::string test) {
            _log.debug(test);
        });
        //f2.wait();
        threadpool.then(f2, [this]() {
            _log.debug("1234");
        });
        //f3.wait();
    });

    _client->set_close_handler([this](websocket_close_status close_status,
                                      utility::string_t reason,
                                      std::error_code error) {
        on_websocket_disconnnect(close_status, to_utf8string(reason), error);
    });
}

DiscordCPP::Gateway::~Gateway() {
    _keepalive = false;
    delete _client;
}

void DiscordCPP::Gateway::set_message_handler(
    const std::function<void(web::json::value)>& handler) {
    _message_handler = handler;
}

pplx::task<void> DiscordCPP::Gateway::connect(const std::string& url) {
    _url = url;
    _log.info("connecting to websocket: " + url);
    return _client->connect(to_string_t(url)).then([this] {
        _connected = true;
        start_heartbeating();
    });
}

///@throws	ClientException
pplx::task<void> DiscordCPP::Gateway::send(const value& message) {
    if (_connected == false) throw ClientException("Gateway not connected");

    web::websockets::client::websocket_outgoing_message msg;
    msg.set_utf8_message(to_utf8string(message.serialize()));

    return _client->send(msg);
}

pplx::task<void> DiscordCPP::Gateway::close() {
    _keepalive = false;
    _connected = false;
    _client->set_close_handler(
        [](websocket_close_status, utility::string_t, std::error_code) {});
    return _client->close();
}
