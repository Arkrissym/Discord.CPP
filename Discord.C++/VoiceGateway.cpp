#include "VoiceGateway.h"

using namespace web::json;
using namespace utility::conversions;

value DiscordCPP::VoiceGateway::get_heartbeat_payload() {
    value payload = value();

    payload[U("op")] = value(3);
    payload[U("d")] = value(time(NULL));

    return payload;
}

pplx::task<void> DiscordCPP::VoiceGateway::identify() {
    return pplx::create_task([this] {
        value out_payload;

        out_payload[U("d")][U("server_id")] = value(to_string_t(_guild_id));
        out_payload[U("d")][U("session_id")] = value(to_string_t(_session_id));
        out_payload[U("d")][U("token")] = value(to_string_t(_token));

        if (_resume == true) {
            out_payload[U("op")] = value(7);

            _resume = false;

            this->send(out_payload)
                .then([this] { _log.info("Resume payload has been sent"); })
                .wait();
        } else {
            out_payload[U("op")] = value(0);
            out_payload[U("d")][U("user_id")] = value(to_string_t(_user_id));

            this->send(out_payload)
                .then([this] { _log.info("Identify payload has been sent"); })
                .wait();
        }
    });
}

void DiscordCPP::VoiceGateway::on_websocket_incoming_message(
    const web::json::value& payload) {
    int op = payload.at(U("op")).as_integer();

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
            _heartbeat_interval = (int)(payload.at(U("d"))
                                            .at(U("heartbeat_interval"))
                                            .as_integer());
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
                                       const std::string& user_id)
    : Gateway(token) {
    _log = Logger("Discord.VoiceGateway (guild id: " + guild_id + ")");

    _session_id = session_id;
    _guild_id = guild_id;
    _user_id = user_id;
}

DiscordCPP::VoiceGateway::~VoiceGateway() {}
