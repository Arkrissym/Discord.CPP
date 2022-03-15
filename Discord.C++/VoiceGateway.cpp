#include "VoiceGateway.h"

json DiscordCPP::VoiceGateway::get_heartbeat_payload() {
    return {{"op", 3}, {"d", std::to_string(time(NULL))}};
}

void DiscordCPP::VoiceGateway::identify() {
    json out_payload = {
        {
            "d", {
                     {"server_id", _guild_id},     //
                     {"session_id", _session_id},  //
                     {"token", _token}             //
                 }                                 //
        }                                          //
    };

    if (_resume == true) {
        out_payload["op"] = 7;

        _resume = false;

        this->send(out_payload).wait();
        _log.info("Resume payload has been sent");
    } else {
        out_payload["op"] = 0;
        out_payload["d"]["user_id"] = _user_id;

        this->send(out_payload).wait();
        _log.info("Identify payload has been sent");
    }
}

void DiscordCPP::VoiceGateway::on_websocket_incoming_message(
    const std::string& message) {
    _log.debug("Received message: " + message);

    json payload = json::parse(message);
    int op = payload["op"].get<int>();

    switch (op) {
        case 2:
            _reconnect_timeout = 0;
            _last_heartbeat_ack = time(0);
            _resume = true;
            _log.info("connected to: " + _url);
            break;
        case 6:
            _log.debug("received heartbeat ack");
            _last_heartbeat_ack = time(0);
            break;
        case 8:
            _heartbeat_interval = payload["d"]["heartbeat_interval"].get<int>();
            identify();
            break;
        case 9:
            _reconnect_timeout = 0;
            _last_heartbeat_ack = time(0);
            _resume = true;
            _log.info("successfully resumed session for guild with id: " +
                      _guild_id);
            break;
    }

    _message_handler(payload);
}

DiscordCPP::VoiceGateway::VoiceGateway(const std::string& token,
                                       const std::string& session_id,
                                       const std::string& guild_id,
                                       const std::string& user_id,
                                       const std::shared_ptr<Threadpool>& threadpool)
    : Gateway(token, threadpool) {
    _log = Logger("Discord.VoiceGateway (guild id: " + guild_id + ")");

    _session_id = session_id;
    _guild_id = guild_id;
    _user_id = user_id;
}
