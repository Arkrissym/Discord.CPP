#pragma once
#include <cpprest/json.h>
#include <cpprest/ws_client.h>
#include "Logger.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

	class Gateway {
		protected:
			///websocket client
			web::websockets::client::websocket_callback_client *_client;
			///the url of the gateway
			std::string _url;
			///token to identify with the gateway
			std::string _token;
			///heartbeat interval in milliseconds
			unsigned int _heartbeat_interval;
			///wether to keep the websocket alive or not
			bool *_keepalive;
			///sometimes it is better a few seconds before reconnecting...
			unsigned int _reconnect_timeout;
			///timestamp of last heartbeat ack
			time_t _last_heartbeat_ack;
			///heartbeat task
			pplx::task<void> _heartbeat_task;
			Logger _log;
			///the message handler set by using set_message_handler
			std::function<void(web::json::value payload)> _message_handler;

			DLL_EXPORT void start_heartbeating();
			DLL_EXPORT virtual web::json::value get_heartbeat_payload() = 0;
			DLL_EXPORT virtual pplx::task<void> identify() = 0;
			DLL_EXPORT virtual void on_websocket_incoming_message(web::json::value payload) = 0;
			DLL_EXPORT void on_websocket_disconnnect(web::websockets::client::websocket_close_status status, std::string reason, std::error_code error);
		public:
			DLL_EXPORT Gateway(std::string token);
			DLL_EXPORT virtual ~Gateway();

			DLL_EXPORT void set_message_handler(const std::function<void(web::json::value payload)> &handler);

			DLL_EXPORT pplx::task<void> connect(std::string url);
			DLL_EXPORT pplx::task<void> send(web::json::value message);
			DLL_EXPORT pplx::task<void> close();
	};

}
