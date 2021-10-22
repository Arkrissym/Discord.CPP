#include "Discord.h"

#include <stdlib.h>
#include <time.h>

#include <iostream>

#include "static.h"

/**	Creates a Discord instance with one or more shards
	@param[in]	token		Bot token for authentication
	@param[in]	intents		Intents used for the gateway.
	@param[in]	num_shards	(optional) number of shards that exist (default: 0 used for automatic sharding)
*/
DiscordCPP::Discord::Discord(const std::string& token, const Intents& intents, const unsigned int num_shards) : DiscordObject(token), _num_shards(num_shards) {
    id = "0";
    log = Logger("discord");

    if (num_shards == 0) {
        json tmp = api_call("/gateway/bot", web::http::methods::GET, json(), "", false);
        tmp.at("shards").get_to<unsigned int>(_num_shards);
    }

    for (unsigned int i = 0; i < _num_shards; i++) {
        MainGateway* _client = new MainGateway(_token, intents, i, _num_shards);

        _client->set_message_handler([this](json payload) {
            on_websocket_incoming_message(payload);
        });

        _gateways.push_back(_client);
    }

    connect();

    srand((unsigned int)time(NULL));
}

/**	Creates a Discord instance with ONE shard
	@param[in]	token		Bot token for authentication
	@param[in]	intents		Intents used for the gateway.
	@param[in]	shard_id	(optional) the id of the shard (default: 0)
	@param[in]	num_shards	(optional) number of shards that exist (default: 1)
*/
DiscordCPP::Discord::Discord(const std::string& token, const Intents& intents, const unsigned int shard_id, const unsigned int num_shards) : DiscordObject(token), _num_shards(num_shards) {
    id = std::to_string(shard_id);
    log = Logger("discord");

    MainGateway* _client = new MainGateway(_token, intents, shard_id, _num_shards);

    _gateways.push_back(_client);

    connect();

    srand((unsigned int)time(NULL));
}

DiscordCPP::Discord::~Discord() {
    for (unsigned int i = 0; i < _gateways.size(); i++) {
        _gateways[i]->close().wait();
        delete _gateways[i];
    }

    delete _user;
    for (unsigned int i = 0; i < _guilds.size(); i++) {
        delete _guilds[i];
    }
}

///	@param[in]	user	the User
void DiscordCPP::Discord::on_ready(User) {
    log.debug("on_ready");
}

///	@param[in]	message	the Message that was received
void DiscordCPP::Discord::on_message(Message) {
    log.debug("on_message");
}

/**	@param[in]	user	the User who has been banned
	@param[in]	guild	the Guild the User has been banned from
*/
void DiscordCPP::Discord::on_user_ban(User, Guild) {
    log.debug("on_member_ban");
}

/**	@param[in]	user	the User who has been unbanned
	@param[in]	guild	the Guild the User has been unbanned from
*/
void DiscordCPP::Discord::on_user_unban(User, Guild) {
    log.debug("on_member_unban");
}

/**	@param[in]	member	the User who has joined
	@param[in]	guild	the Guild the User has joined
*/
void DiscordCPP::Discord::on_user_join(Member, Guild) {
    log.debug("on_member_join");
}

/**	@param[in]	user	the User who has been removed
	@param[in]	guild	the Guild the User has been removed from
*/
void DiscordCPP::Discord::on_user_remove(User, Guild) {
    log.debug("on_member_remove");
}

/**	@param[in]	user		the User that started typing
	@param[in]	channel		the TextChannel where the USer started typing
	@param[in]	timestamp	(unix time) when the User started typing
*/
void DiscordCPP::Discord::on_typing_start(User, TextChannel, unsigned int) {
    log.debug("on_typing_start");
}

/**	@param[in]	status		the new status (see DiscordStatus)
	@param[in]	activity	(optional) the Activity
	@param[in]	afk			(optional) wether the bot/user is afk or not
	@param[in]	shard_id	(optional) the shard whose presence will be updated (use -1 for all shards, default is -1)
*/
void DiscordCPP::Discord::update_presence(const std::string& status, Activity activity, const bool afk, const int shard_id) {
    json presence = {
        {"op", 3},  //
        {"d", {
                  {"since", {}},       //
                  {"status", status},  //
                  {"afk", afk}         //
              }}                       //
    };

    if (activity.type == ActivityTypes::NoActivity)
        presence["d"]["game"] = {};
    else
        presence["d"]["game"] = activity.to_json();

    if (shard_id == -1) {
        for (unsigned int i = 0; i < _gateways.size(); i++) {
            _gateways[i]->send(presence).wait();
        }
    } else {
        get_shard(shard_id)->send(presence).wait();
    }
}

DiscordCPP::MainGateway* DiscordCPP::Discord::get_shard(unsigned int shard_id) {
    for (unsigned int i = 0; i < _gateways.size(); i++) {
        if (_gateways[i]->get_shard_id() == shard_id)
            return _gateways[i];
    }

    throw ClientException("shard with id " + std::to_string(shard_id) + " not found");

    return NULL;
}

DiscordCPP::Guild* DiscordCPP::Discord::get_guild(const std::string& guild_id) {
    for (size_t i = 0; i < _guilds.size(); i++) {
        if (_guilds[i]->id == guild_id) {
            return _guilds[i];
        }
    }

    throw ClientException("Guild with id " + guild_id + " not found");

    return NULL;
}

void DiscordCPP::Discord::connect() {
    log.info("connecting to websocket: " + std::string(GATEWAY_URL));

    for (unsigned int i = 0; i < _gateways.size(); i++) {
        _gateways[i]->connect(GATEWAY_URL);

        waitFor(std::chrono::milliseconds(10000)).wait();
    }
}

void DiscordCPP::Discord::on_websocket_incoming_message(const json& payload) {
    int op = payload.at("op").get<int>();

    switch (op) {
        case 0: {
            std::string event_name = utility::conversions::to_utf8string(payload.at("t").get<std::string>());
            try {
                handle_raw_event(event_name, payload.at("d"));
            } catch (std::exception& e) {
                log.error("error while handling event " + event_name + ": " + e.what());
            }
            break;
        }
        case 1:
        case 7:
        case 9:
        case 10:
        case 11:
            //already handled by MainGateway
            break;
        default:
            log.debug(payload.dump());

            log.warning("ignoring payload with op-code: " + std::to_string(op));
            break;
    }
}

void DiscordCPP::Discord::handle_raw_event(const std::string& event_name, const json& data) {
    //https://discordapp.com/developers/docs/topics/gateway#commands-and-events-gateway-events
    if (event_name == "READY") {
        _user = new User(data.at("user"), _token);

        //_guilds
        for (json guild : data.at("guilds")) {
            _guilds.push_back(new Guild(this, guild, _token));
        }

        try {
            on_ready(User(*_user));
        } catch (const std::exception& e) {
            log.error("ignoring exception in on_ready: " + std::string(e.what()));
        }
    } else if (event_name == "CHANNEL_CREATE") {
        if (has_value(data, "guild_id")) {
            std::string guild_id = data.at("guild_id").get<std::string>();
            Channel* channel = Channel::from_json(this, data, _token);

            get_guild(guild_id)->_add_channel(channel);
        }
    } else if (event_name == "CHANNEL_UPDATE") {
        if (has_value(data, "guild_id")) {
            std::string guild_id = data.at("guild_id").get<std::string>();
            Channel* channel = Channel::from_json(this, data, _token);

            get_guild(guild_id)->_update_channel(channel);
        }
    } else if (event_name == "CHANNEL_DELETE") {
        if (has_value(data, "guild_id")) {
            std::string guild_id = data.at("guild_id").get<std::string>();
            std::string channel_id = data.at("id").get<std::string>();

            get_guild(guild_id)->_remove_channel(channel_id);
        }
    } else if (event_name == "CHANNEL_PINS_UPDATE") {
        if (has_value(data, "channel_id") && has_value(data, "last_pin_timestamp")) {
            std::string channel_id = data.at("channel_id").get<std::string>();
            std::string last_pin = data.at("last_pin_timestamp").get<std::string>();

            for (unsigned int i = 0; i < _guilds.size(); i++) {
                for (unsigned int j = 0; j < _guilds[i]->channels.size(); j++) {
                    if (_guilds[i]->channels[j]->id == channel_id) {
                        ((TextChannel*)_guilds[i]->channels[j])->last_pin_timestamp = last_pin;
                        return;
                    }
                }
            }
        }
    } else if (event_name == "GUILD_CREATE") {
        Guild* tmp_guild = new Guild(this, data, _token);

        for (unsigned int i = 0; i < _guilds.size(); i++) {
            if (tmp_guild->id == _guilds[i]->id) {
                Guild* old = _guilds[i];
                _guilds[i] = tmp_guild;
                delete old;
                log.debug("Updated guild data");
                return;
            }
        }

        _guilds.push_back(tmp_guild);
        log.debug("data of new guild added");
    } else if (event_name == "GUILD_UPDATE") {
        Guild* tmp_guild = new Guild(this, data, _token);

        for (unsigned int i = 0; i < _guilds.size(); i++) {
            if (tmp_guild->id == _guilds[i]->id) {
                Guild* old = _guilds[i];
                _guilds[i] = tmp_guild;
                delete old;
                log.debug("Updated guild data");
                return;
            }
        }
    } else if (event_name == "GUILD_DELETE") {
        std::string guild_id = data.at("guild_id").get<std::string>();

        for (unsigned int i = 0; i < _guilds.size(); i++) {
            if (_guilds[i]->id == guild_id) {
                if (has_value(data, "unavailable") && (data.at("unavailable").get<bool>() == true)) {
                    _guilds[i]->unavailable = true;
                } else {
                    _guilds.erase(_guilds.begin() + i);
                }
                break;
            }
        }
    } else if (event_name == "GUILD_BAN_ADD") {
        Guild guild = Guild(*get_guild(data.at("guild_id").get<std::string>()));
        User user = User(data.at("user"), _token);

        try {
            on_user_ban(user, guild);
        } catch (const std::exception& e) {
            log.error("ignoring exception in on_user_ban: " + std::string(e.what()));
        }
    } else if (event_name == "GUILD_BAN_REMOVE") {
        Guild guild = Guild(*get_guild(data.at("guild_id").get<std::string>()));
        User user = User(data.at("user"), _token);

        try {
            on_user_unban(user, guild);
        } catch (const std::exception& e) {
            log.error("ignoring exception in on_user_unban: " + std::string(e.what()));
        }
    } else if (event_name == "GUILD_MEMBER_ADD") {
        if (has_value(data, ("guild_id"))) {
            Guild* guild = get_guild(data.at("guild_id").get<std::string>());
            Member* member = new Member(data, _token);

            guild->_add_member(member);

            try {
                on_user_join(Member(*member), Guild(*guild));
            } catch (const std::exception& e) {
                log.error("ignoring exception on on_member_join: " + std::string(e.what()));
            }
        }
    } else if (event_name == "GUILD_MEMBER_UPDATE") {
        if (has_value(data, ("guild_id"))) {
            std::string guild_id = data.at("guild_id").get<std::string>();
            Member* member = new Member(data, _token);

            get_guild(guild_id)->_update_member(member);
        }
    } else if (event_name == "GUILD_MEMBER_REMOVE") {
        if (has_value(data, ("guild_id"))) {
            Guild* guild = get_guild(data.at("guild_id").get<std::string>());
            User user = User(data.at("user"), _token);

            guild->_remove_member(user.id);

            try {
                on_user_remove(user, Guild(*guild));
            } catch (const std::exception& e) {
                log.error("ignoring exception on on_member_join: " + std::string(e.what()));
            }
        }
    } else if (event_name == "MESSAGE_CREATE") {
        if (has_value(data, ("guild_id"))) {
            std::string guild_id = data.at("guild_id").get<std::string>();
            std::string channel_id = data.at("channel_id").get<std::string>();
            std::string msg_id = data.at("id").get<std::string>();

            for (unsigned int i = 0; i < _guilds.size(); i++) {
                if (_guilds[i]->id == guild_id) {
                    for (unsigned int j = 0; j < _guilds[i]->channels.size(); j++) {
                        if (_guilds[i]->channels[j]->id == channel_id) {
                            ((TextChannel*)_guilds[i]->channels[j])->last_message_id = msg_id;
                            break;
                        }
                    }

                    break;
                }
            }
        }

        try {
            on_message(Message(data, _token));
        } catch (const std::exception& e) {
            log.error("ignoring exception in on_message: " + std::string(e.what()));
        }
    } else if (event_name == "TYPING_START") {
        std::string channel_id = data.at("channel_id").get<std::string>();
        User user;
        if (has_value(data, ("user_id"))) {
            user = User(data.at("user_id").get<std::string>(), _token);
        } else {
            user = User(data.at("user"), _token);
        }
        TextChannel channel = TextChannel(channel_id, _token);
        unsigned int timestamp = data.at("timestamp").get<unsigned int>();

        try {
            on_typing_start(user, channel, timestamp);
        } catch (const std::exception& e) {
            log.error("ignoring exception in on_typing_start: " + std::string(e.what()));
        }
    } else if (event_name == "VOICE_STATE_UPDATE") {
        if (_user->id != data.at("user_id").get<std::string>()) {
            return;
        }

        VoiceState* voice_state = NULL;
        for (unsigned int i = 0; i < _voice_states.size(); i++) {
            if (_voice_states[i]->guild_id == data.at("guild_id").get<std::string>()) {
                voice_state = _voice_states[i];
                break;
            }
        }

        if (voice_state == NULL) {
            voice_state = new VoiceState();
            voice_state->guild_id = data.at("guild_id").get<std::string>();

            _voice_states.push_back(voice_state);
        }

        if (!(has_value(data, "channel_id"))) {
            voice_state->channel_id = "";
            voice_state->session_id = "";
            voice_state->endpoint = "";
            voice_state->voice_token = "";
        } else {
            voice_state->channel_id = data.at("channel_id").get<std::string>();
            voice_state->session_id = data.at("session_id").get<std::string>();
        }
    } else if (event_name == "VOICE_SERVER_UPDATE") {
        VoiceState* voice_state = NULL;
        for (unsigned int i = 0; i < _voice_states.size(); i++) {
            if (_voice_states[i]->guild_id == data.at("guild_id").get<std::string>()) {
                voice_state = _voice_states[i];
                break;
            }
        }

        if (voice_state == NULL) {
            voice_state = new VoiceState();
            voice_state->guild_id = data.at("guild_id").get<std::string>();

            _voice_states.push_back(voice_state);
        }

        voice_state->endpoint = "wss://" + data.at("endpoint").get<std::string>();
        voice_state->endpoint += "?v=4";
        voice_state->voice_token = data.at("token").get<std::string>();
    } else {
        log.warning("ignoring event: " + event_name);
        log.debug(data.dump());
    }
}
