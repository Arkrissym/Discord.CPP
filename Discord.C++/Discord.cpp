#include "Discord.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "static.h"

/**	Creates a Discord instance with one or more shards
    @param[in]	token		Bot token for authentication
    @param[in]	intents		Intents used for the gateway.
    @param[in]	num_shards	(optional) number of shards that exist (default: 0 used for automatic sharding)
*/
DiscordCPP::Discord::Discord(const std::string &token, const Intents &intents, const unsigned int num_shards)
    : DiscordObject(token, "0"),
      _num_shards(num_shards) {
    log = Logger("discord");

    if (num_shards == 0) {
        json tmp = api_call("/gateway/bot", "GET", json(), "", false);
        tmp.at("shards").get_to<unsigned int>(_num_shards);
    }

    json app = api_call("/oauth2/applications/@me", "GET");
    app.at("id").get_to(_application_id);

    for (unsigned int i = 0; i < _num_shards; i++) {
        std::shared_ptr<MainGateway> _client = std::make_shared<MainGateway>(get_token(), intents, i, _num_shards);

        _client->set_message_handler([this](const json &payload) { on_websocket_incoming_message(payload); });

        _gateways.push_back(_client);
    }
}

/**	Creates a Discord instance with ONE shard
    @param[in]	token		Bot token for authentication
    @param[in]	intents		Intents used for the gateway.
    @param[in]	shard_id	(optional) the id of the shard (default: 0)
    @param[in]	num_shards	(optional) number of shards that exist (default: 1)
*/
DiscordCPP::Discord::Discord(const std::string &token, const Intents &intents, const unsigned int shard_id, const unsigned int num_shards)
    : DiscordObject(token, std::to_string(shard_id)), _num_shards(num_shards) {
    log = Logger("discord");

    json app = api_call("/oauth2/applications/@me", "GET");
    app.at("id").get_to(_application_id);

    std::shared_ptr<MainGateway> _client = std::make_shared<MainGateway>(get_token(), intents, shard_id, _num_shards);
    _client->set_message_handler([this](const json &payload) { on_websocket_incoming_message(payload); });

    _gateways.push_back(_client);
}

DiscordCPP::Discord::~Discord() {
    for (auto &_gateway : _gateways) {
        _gateway->close().get();
    }

    delete _user;
    for (auto &_guild : _guilds) {
        delete _guild;
    }
}

void DiscordCPP::Discord::start() {
    srand((unsigned int)time(nullptr));
    connect();
}

///	@param[in]	user	the User
void DiscordCPP::Discord::on_ready(User) { log.debug("on_ready"); }

///	@param[in]	message	the Message that was received
void DiscordCPP::Discord::on_message(Message) { log.debug("on_message"); }

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

/// @param[in]  interaction the Interaction that was received
void DiscordCPP::Discord::on_interaction(Interaction) {
    log.debug("on_interaction");
}

/**	@param[in]	status		the new status (see DiscordStatus)
    @param[in]	activity	(optional) the Activity
    @param[in]	afk			(optional) wether the bot/user is afk or not
    @param[in]	shard_id	(optional) the shard whose presence will be updated (use -1 for all shards, default is -1)
*/
void DiscordCPP::Discord::update_presence(const std::string &status, Activity activity, const bool afk, const int shard_id) {
    json presence = {
        {"op", 3},  //
        {"d",
         {
             {"since", {}},       //
             {"status", status},  //
             {"afk", afk}         //
         }}                       //
    };

    if (activity.get_type() == Activity::NoActivity)
        presence["d"]["game"] = {};
    else
        presence["d"]["game"] = activity.to_json();

    if (shard_id == -1) {
        for (auto &_gateway : _gateways) {
            _gateway->send(presence).get();
        }
    } else {
        get_shard(shard_id)->send(presence).get();
    }
}

/** @return a vector containing all global application commands
 */
std::vector<DiscordCPP::ApplicationCommand> DiscordCPP::Discord::get_application_commands() {
    std::vector<ApplicationCommand> commands;

    json tmp = api_call("/applications/" + _application_id + "/commands");
    for (json command : tmp) {
        commands.emplace_back(command, get_token());
    }

    return commands;
}

/** @param[in]  guild   the Guild to get the application commands for
    @return a vector containing all application commands of the specified guild
 */
std::vector<DiscordCPP::ApplicationCommand> DiscordCPP::Discord::get_application_commands(const Guild &guild) {
    std::vector<ApplicationCommand> commands;

    json tmp = api_call("/applications/" + _application_id + "/guilds/" + guild.get_id() + "/commands");
    for (json command : tmp) {
        commands.emplace_back(command, get_token());
    }

    return commands;
}

DiscordCPP::ApplicationCommand DiscordCPP::Discord::create_application_command(
    ApplicationCommand command) {
    json data = command.to_json();

    std::string url = "/applications/" + _application_id;
    if (command.get_guild_id().has_value()) {
        url += "/guilds/" + command.get_guild_id().value();
    }
    url += "/commands";

    return {api_call(url, "POST", data, "application/json", false), get_token()};
}

std::shared_ptr<DiscordCPP::MainGateway> DiscordCPP::Discord::get_shard(
    unsigned int shard_id) {
    for (auto &_gateway : _gateways) {
        if (_gateway->get_shard_id() == shard_id) {
            return _gateway;
        }
    }

    throw ClientException("shard with id " + std::to_string(shard_id) + " not found");
}

DiscordCPP::Guild *DiscordCPP::Discord::get_guild(const std::string &guild_id) {
    for (auto &_guild : _guilds) {
        if (_guild->get_id() == guild_id) {
            return _guild;
        }
    }

    throw ClientException("Guild with id " + guild_id + " not found");

    return nullptr;
}

DiscordCPP::VoiceState *DiscordCPP::Discord::get_voice_state(
    const std::string &user_id, const std::string &guild_id) {
    VoiceState *voice_state = nullptr;
    if (_voice_states.find(user_id) != _voice_states.end()) {
        for (auto it : _voice_states.at(user_id)) {
            if (it->guild_id == guild_id) {
                voice_state = it;
                break;
            }
        }
    }
    return voice_state;
}

void DiscordCPP::Discord::connect() {
    log.info("connecting to websocket: " + std::string(GATEWAY_URL));

    for (auto &_gateway : _gateways) {
        _gateway->connect(GATEWAY_URL);

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void DiscordCPP::Discord::on_websocket_incoming_message(const json &payload) {
    int op = payload.at("op").get<int>();

    switch (op) {
        case 0: {
            std::string event_name = payload.at("t").get<std::string>();
            try {
                handle_raw_event(event_name, payload.at("d"));
            } catch (std::exception &e) {
                log.error("error while handling event " + event_name + ": " +
                          e.what());
            }
            break;
        }
        case 1:
        case 7:
        case 9:
        case 10:
        case 11:
            // already handled by MainGateway
            break;
        default:
            log.debug(payload.dump());

            log.warning("ignoring payload with op-code: " + std::to_string(op));
            break;
    }
}

void DiscordCPP::Discord::handle_raw_event(const std::string &event_name,
                                           const json &data) {
    // https://discordapp.com/developers/docs/topics/gateway#commands-and-events-gateway-events
    if (event_name == "READY") {
        _user = new User(data.at("user"), get_token());

        //_guilds
        for (json guild : data.at("guilds")) {
            _guilds.push_back(new Guild(this, guild, get_token()));
        }

        try {
            on_ready(User(*_user));
        } catch (const std::exception &e) {
            log.error("ignoring exception in on_ready: " +
                      std::string(e.what()));
        }
    } else if (event_name == "CHANNEL_CREATE") {
        if (has_value(data, "guild_id")) {
            std::string guild_id = data.at("guild_id").get<std::string>();
            Channel *channel = Channel::from_json(this, data, get_token());

            get_guild(guild_id)->_add_channel(channel);
        }
    } else if (event_name == "CHANNEL_UPDATE") {
        if (has_value(data, "guild_id")) {
            std::string guild_id = data.at("guild_id").get<std::string>();
            Channel *channel = Channel::from_json(this, data, get_token());

            get_guild(guild_id)->_update_channel(channel);
        }
    } else if (event_name == "CHANNEL_DELETE") {
        if (has_value(data, "guild_id")) {
            std::string guild_id = data.at("guild_id").get<std::string>();
            std::string channel_id = data.at("id").get<std::string>();

            get_guild(guild_id)->_remove_channel(channel_id);
        }
    } else if (event_name == "CHANNEL_PINS_UPDATE") {
        if (has_value(data, "channel_id") &&
            has_value(data, "last_pin_timestamp")) {
            std::string channel_id = data.at("channel_id").get<std::string>();
            std::string last_pin =
                data.at("last_pin_timestamp").get<std::string>();

            for (auto &_guild : _guilds) {
                for (auto &channel : _guild->channels) {
                    if (channel->get_id() == channel_id) {
                        ((TextChannel *)channel)->_set_last_pin_timestamp(last_pin);
                        return;
                    }
                }
            }
        }
    } else if ((event_name == "GUILD_CREATE") ||
               (event_name == "GUILD_UPDATE")) {
        Guild *tmp_guild = new Guild(this, data, get_token());

        if (has_value(data, "voice_states")) {
            for (json voice_state : data.at("voice_states")) {
                voice_state["guild_id"] = tmp_guild->get_id();
                _process_voice_state_update(voice_state);
            }
        }

        for (auto &_guild : _guilds) {
            if (tmp_guild->get_id() == _guild->get_id()) {
                Guild *old = _guild;
                _guild = tmp_guild;
                delete old;
                log.debug("Updated guild data");
                return;
            }
        }

        _guilds.push_back(tmp_guild);
        log.debug("data of new guild added");
    } else if (event_name == "GUILD_DELETE") {
        std::string guild_id = data.at("guild_id").get<std::string>();

        for (unsigned int i = 0; i < _guilds.size(); i++) {
            if (_guilds[i]->get_id() == guild_id) {
                if (has_value(data, "unavailable") &&
                    (data.at("unavailable").get<bool>() == true)) {
                    _guilds[i]->unavailable = true;
                } else {
                    _guilds.erase(_guilds.begin() + i);
                }
                break;
            }
        }
    } else if (event_name == "GUILD_BAN_ADD") {
        Guild guild = Guild(*get_guild(data.at("guild_id").get<std::string>()));
        User user = User(data.at("user"), get_token());

        try {
            on_user_ban(user, guild);
        } catch (const std::exception &e) {
            log.error("ignoring exception in on_user_ban: " +
                      std::string(e.what()));
        }
    } else if (event_name == "GUILD_BAN_REMOVE") {
        Guild guild = Guild(*get_guild(data.at("guild_id").get<std::string>()));
        User user = User(data.at("user"), get_token());

        try {
            on_user_unban(user, guild);
        } catch (const std::exception &e) {
            log.error("ignoring exception in on_user_unban: " +
                      std::string(e.what()));
        }
    } else if (event_name == "GUILD_MEMBER_ADD") {
        if (has_value(data, ("guild_id"))) {
            Guild *guild = get_guild(data.at("guild_id").get<std::string>());
            Member member = Member(data, get_token());

            guild->_add_member(member);

            try {
                on_user_join(Member(member), Guild(*guild));
            } catch (const std::exception &e) {
                log.error("ignoring exception on on_member_join: " +
                          std::string(e.what()));
            }
        }
    } else if (event_name == "GUILD_MEMBER_UPDATE") {
        if (has_value(data, ("guild_id"))) {
            std::string guild_id = data.at("guild_id").get<std::string>();
            Member member = Member(data, get_token());

            get_guild(guild_id)->_update_member(member);
        }
    } else if (event_name == "GUILD_MEMBER_REMOVE") {
        if (has_value(data, ("guild_id"))) {
            Guild *guild = get_guild(data.at("guild_id").get<std::string>());
            User user = User(data.at("user"), get_token());

            guild->_remove_member(user.get_id());

            try {
                on_user_remove(user, Guild(*guild));
            } catch (const std::exception &e) {
                log.error("ignoring exception on on_member_join: " +
                          std::string(e.what()));
            }
        }
    } else if (event_name == "MESSAGE_CREATE") {
        if (has_value(data, ("guild_id"))) {
            std::string guild_id = data.at("guild_id").get<std::string>();
            std::string channel_id = data.at("channel_id").get<std::string>();
            std::string msg_id = data.at("id").get<std::string>();

            for (auto &_guild : _guilds) {
                if (_guild->get_id() == guild_id) {
                    for (auto &channel : _guild->channels) {
                        if (channel->get_id() == channel_id) {
                            ((TextChannel *)channel)->_set_last_message_id(msg_id);
                            break;
                        }
                    }

                    break;
                }
            }
        }

        try {
            on_message(Message(data, get_token()));
        } catch (const std::exception &e) {
            log.error("ignoring exception in on_message: " + std::string(e.what()));
        }
    } else if (event_name == "TYPING_START") {
        std::string channel_id = data.at("channel_id").get<std::string>();
        User user;
        if (has_value(data, ("user_id"))) {
            user = User(data.at("user_id").get<std::string>(), get_token());
        } else {
            user = User(data.at("user"), get_token());
        }
        TextChannel channel = TextChannel(channel_id, get_token());
        unsigned int timestamp = data.at("timestamp").get<unsigned int>();

        try {
            on_typing_start(user, channel, timestamp);
        } catch (const std::exception &e) {
            log.error("ignoring exception in on_typing_start: " + std::string(e.what()));
        }
    } else if (event_name == "VOICE_STATE_UPDATE") {
        _process_voice_state_update(data);
    } else if (event_name == "VOICE_SERVER_UPDATE") {
        VoiceState *voice_state = get_voice_state(
            _user->get_id(), data.at("guild_id").get<std::string>());

        if (voice_state == nullptr) {
            voice_state = new VoiceState();
            voice_state->guild_id = data.at("guild_id").get<std::string>();

            if (_voice_states.find(_user->get_id()) != _voice_states.end()) {
                _voice_states.at(_user->get_id()).push_back(voice_state);
            } else {
                std::vector<VoiceState *> voice_states;
                voice_states.push_back(voice_state);
                _voice_states.insert(std::pair<std::string, std::vector<VoiceState *>>(_user->get_id(), voice_states));
            }
        }

        voice_state->endpoint = "wss://" + data.at("endpoint").get<std::string>();
        voice_state->endpoint += "?v=4";
        voice_state->voice_token = data.at("token").get<std::string>();
    } else if (event_name == "INTERACTION_CREATE") {
        try {
            on_interaction(Interaction(data, get_token()));
        } catch (const std::exception &e) {
            log.error("ignoring exception in on_interaction: " +
                      std::string(e.what()));
        }
    } else {
        log.debug("ignoring event: " + event_name);
        log.debug(data.dump());
    }
}

void DiscordCPP::Discord::_process_voice_state_update(const json &data) {
    std::string user_id = data.at("user_id").get<std::string>();

    VoiceState *voice_state =
        get_voice_state(user_id, data.at("guild_id").get<std::string>());

    if (voice_state == nullptr) {
        voice_state = new VoiceState();
        voice_state->guild_id = data.at("guild_id").get<std::string>();

        if (_voice_states.find(user_id) != _voice_states.end()) {
            _voice_states.at(user_id).push_back(voice_state);
        } else {
            std::vector<VoiceState *> voice_states;
            voice_states.push_back(voice_state);
            _voice_states.insert(
                std::pair<std::string, std::vector<VoiceState *>>(
                    user_id, voice_states));
        }
    }

    if (!(has_value(data, "channel_id"))) {
        voice_state->channel_id = "";
        voice_state->session_id = "";
        voice_state->endpoint = "";
        voice_state->voice_token = "";
        if (voice_state->voice_client != nullptr) {
            log.debug("disconnecting voice client");
            voice_state->voice_client->disconnect();
            voice_state->voice_client = nullptr;
        }
    } else {
        voice_state->channel_id = data.at("channel_id").get<std::string>();
        voice_state->session_id = data.at("session_id").get<std::string>();
    }

    log.debug("Updated voice state for user " + user_id);
}
