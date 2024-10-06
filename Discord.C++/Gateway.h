#pragma once
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <nlohmann/json.hpp>

#include "Logger.h"
#include "Threadpool.h"
#include "static.h"

namespace DiscordCPP {

class Gateway {
   protected:
    /// the threadpool used for tasks handling messages
    std::shared_ptr<Threadpool> threadpool;
    /// boost io context used by the websocket client
    boost::asio::io_context io_context;
    /// ssl context used by the websocket client
    boost::asio::ssl::context ssl_context;
    /// buffer for websocket connection
    boost::beast::flat_buffer buffer;
    /// websocket client
    std::unique_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>> _client;
    /// the url of the gateway
    std::string _url;
    /// the url used for reconnecting
    std::string _resume_url;
    /// token to identify with the gateway
    std::string _token;
    /// heartbeat interval in milliseconds
    unsigned int _heartbeat_interval;
    /// wether to keep the websocket alive or not
    bool _keepalive;
    /// sometimes it is better a few seconds before reconnecting...
    unsigned int _reconnect_timeout;
    /// timestamp of last heartbeat ack
    time_t _last_heartbeat_ack = 0;
    /// heartbeat task
    std::thread _heartbeat_task;
    /// indicator if Gateway is connected
    bool _connected;
    /// current sequence number
    unsigned int _sequence_number;
    /// logging instance
    Logger _log;
    /// the message handler set by using set_message_handler
    std::function<void(json payload)> _message_handler;

    DLL_EXPORT void start_heartbeating();
    DLL_EXPORT virtual json get_heartbeat_payload() = 0;
    DLL_EXPORT virtual void identify() = 0;
    DLL_EXPORT void on_read(boost::system::error_code error_code, std::size_t bytes);
    DLL_EXPORT virtual void on_websocket_incoming_message(const std::string& message) = 0;
    DLL_EXPORT void on_websocket_disconnnect();

   public:
    DLL_EXPORT Gateway(std::string token, const std::shared_ptr<Threadpool>& threadpool);
    DLL_EXPORT virtual ~Gateway();

    DLL_EXPORT void set_message_handler(const std::function<void(json payload)>& handler);

    DLL_EXPORT virtual void connect(const std::string& url);
    DLL_EXPORT SharedFuture<void> send(const json& message);
    DLL_EXPORT void close();
};

}  // namespace DiscordCPP
