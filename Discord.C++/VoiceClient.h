#pragma once
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/bind.hpp>

#include "AudioSource.h"
#include "Logger.h"
#include "MainGateway.h"
#include "VoiceGateway.h"
#include "VoiceState.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

using namespace boost::asio::ip;

class udp_client {
   private:
    Logger _log;

    boost::asio::io_service _io_service;
    std::unique_ptr<udp::socket> _socket;
    // udp::socket *_recv_socket;
    udp::endpoint _remote;
    boost::array<char, 128> _recv_buffer{};

    // void wait_for_receive();
    // void handle_receive(boost::system::error_code &error, size_t
    // bytes_tranferred);
   public:
    DLL_EXPORT udp_client(const std::string& ip, const int port);
    DLL_EXPORT ~udp_client();
    DLL_EXPORT void send(const std::string& msg);
    DLL_EXPORT std::string receive();
};

class VoiceClient {
   private:
    std::shared_ptr<Threadpool> threadpool;

    std::string _voice_token;
    std::string _endpoint;
    std::string _session_id;
    std::string _guild_id;
    std::string _channel_id;
    std::string _user_id;

    bool _ready = false;
    bool _playing = false;
    bool _cancel_playing = false;

    unsigned int _ssrc = 0;
    std::string _server_ip;
    unsigned short _server_port = 0;
    std::string _my_ip;
    unsigned short _my_port = 0;

    std::string _mode;
    std::vector<unsigned char> _secret_key;

    std::shared_ptr<MainGateway> _main_ws;
    std::unique_ptr<VoiceGateway> _voice_ws;

    std::unique_ptr<udp_client> _udp;

    unsigned short _sequence = 0;
    unsigned int _timestamp = 0;

    Logger _log;

    DLL_EXPORT void connect_voice_udp();
    DLL_EXPORT void select_protocol();
    DLL_EXPORT void load_session_description(const json& data);
    DLL_EXPORT void speak(bool speak = true);

   public:
    DLL_EXPORT VoiceClient(std::shared_ptr<MainGateway> main_ws, const std::string& voice_token, const std::string& endpoint, const std::string& session_id, std::string guild_id, std::string channel_id, const std::string& user_id);
    DLL_EXPORT VoiceClient() = default;
    DLL_EXPORT ~VoiceClient();

    /** play an AudioSource
        @throws	OpusError	ClientException
    */
    DLL_EXPORT SharedFuture<void> play(AudioSource* source);
    DLL_EXPORT void stop_playing();
    DLL_EXPORT SharedFuture<void> disconnect();
};

}  // namespace DiscordCPP
