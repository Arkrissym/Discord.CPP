#include "MainGateway.h"

#include <thread>

#include "static.h"

DiscordCPP::json DiscordCPP::MainGateway::get_heartbeat_payload() {
    return {{"op", 1}, {"d", ((_sequence_number == 0) ? "null" : std::to_string(_sequence_number))}};
}

void DiscordCPP::MainGateway::on_websocket_incoming_message(
    const json& payload) {
    int op = payload["op"].get<int>();

    if ((payload.count("s") > 0) && payload["s"].is_number_integer()) {
        _sequence_number = payload["s"].get<int>();
    }

    switch (op) {
        case 0:
            if (payload["t"].get<std::string>() == "READY") {
                _reconnect_timeout = 0;
                _last_heartbeat_ack = time(0);

                _invalid_session = false;

                _session_id = payload["d"]["session_id"].get<std::string>();

                std::string str = set_trace(payload);

                _log.info("connected to: " + str + " ]");
                _log.info("session id: " + _session_id);
            } else if (payload["t"].get<std::string>() == "RESUMED") {
                _reconnect_timeout = 0;
                _last_heartbeat_ack = time(0);

                std::string str = set_trace(payload);

                _log.info("successfully resumed session " + _session_id +
                          " with trace " + str + " ]");
            }
            break;
        case 1:
            send_heartbeat_ack();
            break;
        case 7:
            _log.info("received opcode 7: reconnecting to the gateway");
            _client->close(
                web::websockets::client::websocket_close_status::normal,
                U("received opcode 7"));
            break;
        case 9:
            _invalid_session = true;
            break;
        case 10:
            _heartbeat_interval = payload["d"]["heartbeat_interval"].get<int>();
            _log.debug("set heartbeat_interval: " +
                       std::to_string(_heartbeat_interval));
            identify();
            break;
        case 11:
            _log.debug("received heartbeat ACK");
            _last_heartbeat_ack = time(0);
            break;
        default:
            break;
    }

    _message_handler(payload);
}

std::shared_future<void> DiscordCPP::MainGateway::send_heartbeat_ack() {
    auto f = this->send({{"op", 11}});
    return threadpool.then(f, [this] {
        _log.debug("Heartbeat ACK message has been sent");
    });
}

void DiscordCPP::MainGateway::identify() {
    if (_sequence_number > 0) {
        unsigned int seq = _sequence_number;

        _log.info("trying to resume session");

        json resume_json = {
            {"op", 6},
            {"d", {{"token", _token}, {"session_id", _session_id}, {"seq", seq}}}};

        this->send(resume_json).wait();
        _log.info("Resume payload has been sent");

        while (_invalid_session == false) {
            if (_sequence_number > seq) {
                return;
            }

            waitFor(std::chrono::milliseconds(100)).wait();
        }

        _log.info("cannot resume session");

        waitFor(std::chrono::milliseconds(1000 + rand() % 4001)).wait();
    }

    json identify_json = {
        {"op", 2},
        {
            "d", {
                     //
                     {"token", _token},                    //
                     {"intents", _intents.getIntents()},   //
                     {"shard", {_shard_id, _num_shards}},  //
                     {"large_threshold", 250}              //
                 }                                         //
        }                                                  //
    };

#ifdef _WIN32
    identify_json["d"]["properties"]["$os"] = "Windows";
#elif __APPLE__
    identify_json["d"]["properties"]["$os"] = "macOS";
#elif __linux__
    identify_json["d"]["properties"]["$os"] = "Linux";
#elif __unix__
    identify_json["d"]["properties"]["$os"] = "Unix";
#elif __posix
    identify_json["d"]["properties"]["$os"] = "POSIX";
#endif
    identify_json["d"]["properties"]["$browser"] = "Discord.C++";
    identify_json["d"]["properties"]["$device"] = "Discord.C++";

    this->send(identify_json).wait();
    _log.info("Identify payload has been sent");
}

std::string DiscordCPP::MainGateway::set_trace(const json& payload) {
    json tmp = payload["d"]["_trace"];
    std::string str = "[ ";
    for (json::iterator it = tmp.begin(); it != tmp.end(); ++it) {
        std::string t = (*it).get<std::string>();
        _trace.push_back(t);
        if (it == tmp.begin())
            str = str + t;
        else
            str = str + ", " + t;
    }
    return str;
}

DiscordCPP::MainGateway::MainGateway(const std::string& token,
                                     const Intents& intents, const int shard_id,
                                     const unsigned int num_shards)
    : Gateway::Gateway(token, std::thread::hardware_concurrency() / num_shards) {
    _log =
        Logger("Discord.MainGateway (shard id: [" + std::to_string(shard_id) +
               ", " + std::to_string(num_shards) + "])");

    _invalid_session = false;

    _shard_id = shard_id;
    _num_shards = num_shards;

    _sequence_number = 0;

    _intents = intents;

    _message_handler = [this](json) {
        _log.info("dummy message handler called");
    };
}

unsigned int DiscordCPP::MainGateway::get_shard_id() { return _shard_id; }
