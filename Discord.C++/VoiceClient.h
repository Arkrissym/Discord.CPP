#pragma once
#include <cpprest/json.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/bind.hpp>

#include "AudioSource.h"
#include "Logger.h"
#include "MainGateway.h"
#include "VoiceGateway.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

using namespace std;
using namespace web::json;
using namespace utility;
using namespace web::websockets::client;
using namespace boost::asio::ip;

class udp_client {
   private:
    Logger _log;

    boost::asio::io_service _io_service;
    udp::socket* _socket;
    // udp::socket *_recv_socket;
    udp::endpoint _remote;
    boost::array<char, 128> _recv_buffer;

    // void wait_for_receive();
    // void handle_receive(boost::system::error_code &error, size_t
    // bytes_tranferred);
   public:
    DLL_EXPORT udp_client(const string_t& ip, const int port);
    udp_client(udp_client& old) = delete;
    udp_client& operator=(const udp_client&) = delete;
    DLL_EXPORT ~udp_client();
    // DLL_EXPORT void connect(string_t ip, int port);
    DLL_EXPORT void send(const string& msg);
    DLL_EXPORT string receive();
};

class VoiceClient {
   private:
    Threadpool threadpool;

    string_t _voice_token;
    string_t _endpoint;
    string_t _session_id;
    string_t _guild_id;
    string_t _channel_id;
    string_t _user_id;

    bool _ready = false;

    unsigned int _ssrc = 0;
    string_t _server_ip;
    unsigned short _server_port = 0;
    string_t _my_ip;
    unsigned short _my_port = 0;

    string_t _mode;
    vector<unsigned char> _secret_key;

    MainGateway** _main_ws;
    VoiceGateway* _voice_ws;

    udp_client* _udp;

    unsigned short _sequence = 0;
    unsigned int _timestamp = 0;

    Logger _log;

    DLL_EXPORT std::shared_future<void> connect_voice_udp();
    DLL_EXPORT std::shared_future<void> select_protocol();
    DLL_EXPORT std::shared_future<void> load_session_description(const value& data);
    DLL_EXPORT std::shared_future<void> speak(bool speak = true);
    DLL_EXPORT std::shared_future<void> disconnect();

   public:
    DLL_EXPORT VoiceClient(MainGateway** main_ws, const string_t& voice_token,
                           const string_t& endpoint, const string_t& session_id,
                           const string_t& guild_id, const string_t& channel_id,
                           const string_t& user_id);
    VoiceClient(const VoiceClient& old) = delete;
    DLL_EXPORT VoiceClient(){};
    DLL_EXPORT ~VoiceClient();

    /** play an AudioSource
        @throws	OpusError	ClientException
    */
    DLL_EXPORT std::shared_future<void> play(AudioSource* source);
};

}  // namespace DiscordCPP
