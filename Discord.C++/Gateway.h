#pragma once
#include <cpprest/json.h>
#include <cpprest/ws_client.h>
#include <zlib.h>

#include "Logger.h"
#include "Threadpool.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

class Gateway {
   protected:
    Threadpool threadpool;
    /// websocket client
    web::websockets::client::websocket_callback_client* _client;
    /// zlib control struct
    z_stream zs;
    /// the url of the gateway
    std::string _url;
    /// token to identify with the gateway
    std::string _token;
    /// heartbeat interval in milliseconds
    unsigned int _heartbeat_interval;
    /// wether to keep the websocket alive or not
    bool _keepalive;
    /// sometimes it is better a few seconds before reconnecting...
    unsigned int _reconnect_timeout;
    /// timestamp of last heartbeat ack
    time_t _last_heartbeat_ack;
    /// heartbeat task
    pplx::task<void> _heartbeat_task;
    /// indicator if Gateway is connected
    bool _connected;
    Logger _log;
    /// the message handler set by using set_message_handler
    std::function<void(web::json::value payload)> _message_handler;

    DLL_EXPORT std::string decompress_message(const std::string& message);

    DLL_EXPORT void start_heartbeating();
    DLL_EXPORT virtual web::json::value get_heartbeat_payload() = 0;
    DLL_EXPORT virtual pplx::task<void> identify() = 0;
    DLL_EXPORT virtual void on_websocket_incoming_message(
        const web::json::value& payload) = 0;
    DLL_EXPORT void on_websocket_disconnnect(
        const web::websockets::client::websocket_close_status& status,
        const std::string& reason, const std::error_code& error);

   public:
    DLL_EXPORT Gateway(const std::string& token);
    DLL_EXPORT virtual ~Gateway();

    DLL_EXPORT void set_message_handler(
        const std::function<void(web::json::value payload)>& handler);

    DLL_EXPORT pplx::task<void> connect(const std::string& url);
    DLL_EXPORT pplx::task<void> send(const web::json::value& message);
    DLL_EXPORT pplx::task<void> close();
};

}  // namespace DiscordCPP
