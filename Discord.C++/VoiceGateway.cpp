#include "VoiceGateway.h"

#include <utility>

json DiscordCPP::VoiceGateway::get_heartbeat_payload() {
    return {
        {"op", 3},  //
        {"d", {
                  //
                  {"t", std::to_string(time(nullptr))},                                               //
                  {"seq_ack", ((_sequence_number == 0) ? "null" : std::to_string(_sequence_number))}  //
              }}  //
    };
}

void DiscordCPP::VoiceGateway::identify() {
    json out_payload = {
        {
            "d", {
                     {"server_id", _guild_id},                                                           //
                     {"session_id", _session_id},                                                        //
                     {"token", _token},                                                                  //
                     {"seq_ack", ((_sequence_number == 0) ? "null" : std::to_string(_sequence_number))}  //
                 }  //
        }  //
    };

    if (_resume == true) {
        out_payload["op"] = 7;

        _resume = false;

        this->send(out_payload).get();
        _log.debug("Resume payload has been sent");
    } else {
        out_payload["op"] = 0;
        out_payload["d"]["user_id"] = _user_id;

        this->send(out_payload).get();
        _log.debug("Identify payload has been sent");
    }
}

void DiscordCPP::VoiceGateway::on_websocket_incoming_message(const std::string& message) {
    threadpool->execute([this, message]() {
        _log.debug("Received message: " + message);

        json payload = json::parse(message);
        int op = payload["op"].get<int>();

        if ((payload.count("seq") > 0) && payload["seq"].is_number_integer()) {
            _sequence_number = payload["seq"].get<int>();
        }

        switch (op) {
            case 2:
                _reconnect_timeout = 0;
                _last_heartbeat_ack = time(nullptr);
                _resume = true;
                _log.info("connected to: " + _url);
                break;
            case 6:
                _log.debug("received heartbeat ack");
                _last_heartbeat_ack = time(nullptr);
                break;
            case 8:
                _heartbeat_interval = payload["d"]["heartbeat_interval"].get<int>();
                identify();
                break;
            case 9:
                _reconnect_timeout = 0;
                _last_heartbeat_ack = time(nullptr);
                _resume = true;
                _log.info("successfully resumed session for guild with id: " + _guild_id);
                break;
        }

        _message_handler(payload);
    });
}

DiscordCPP::VoiceGateway::VoiceGateway(const std::string& token,
                                       std::string session_id,
                                       std::string guild_id,
                                       std::string user_id,
                                       const std::shared_ptr<Threadpool>& threadpool)
    : Gateway(token, threadpool),
      _session_id(std::move(session_id)),
      _guild_id(std::move(guild_id)),
      _user_id(std::move(user_id)) {
    _log = Logger("Discord.VoiceGateway (guild id: " + _guild_id + ")");
}
