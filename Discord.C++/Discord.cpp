#include "Discord.h"
#include "static.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace utility;
using namespace web::json;
using namespace std;

/**	Creates a Discord instance with one or more shards
	@param[in]	token		Bot token for authentication
	@param[in]	num_shards	(optional) number of shards that exist (default: 0 used for automatic sharding)
*/
DiscordCPP::Discord::Discord(string token, unsigned int num_shards) {
	_token = conversions::to_string_t(token);
	_num_shards = num_shards; 
	id = "0";
	log = Logger("discord");

	if (num_shards == 0) {
		value tmp = api_call("/gateway/bot", web::http::methods::GET, value(), "", false);
		_num_shards = tmp[U("shards")].as_integer();
	}

	for (unsigned int i = 0; i < _num_shards; i++) {
		MainGateway *_client = new MainGateway(conversions::to_utf8string(_token), i, _num_shards);
	
		_client->set_message_handler([this](value payload) {
			on_websocket_incoming_message(payload);
		});

		_gateways.push_back(_client);
	}

	connect();

	srand((unsigned int)time(NULL));

	//log.debug("created Discord object");
}

/**	Creates a Discord instance with ONE shard
	@param[in]	token		Bot token for authentication
	@param[in]	shard_id	(optional) the id of the shard (default: 0)
	@param[in]	num_shards	(optional) number of shards that exist (default: 1)
*/
DiscordCPP::Discord::Discord(string token, unsigned int shard_id, unsigned int num_shards) {
	_token = conversions::to_string_t(token);
	_num_shards = num_shards;
	id = to_string(shard_id);
	log = Logger("discord");

	MainGateway *_client = new MainGateway(conversions::to_utf8string(_token), shard_id, _num_shards);

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
void DiscordCPP::Discord::on_ready(User user) {
	log.debug("on_ready");
}

///	@param[in]	message	the Message that was received
void DiscordCPP::Discord::on_message(Message message) {
	log.debug("on_message");
}

/**	@param[in]	status		the new status (see DiscordStatus)
	@param[in]	activity	(optional) the Activity
	@param[in]	afk			(optional) wether the bot/user is afk or not
	@param[in]	shard_id	(optional) the shard whose presence will be updated (use -1 for all shards, default is -1)
*/
pplx::task<void> DiscordCPP::Discord::update_presence(string status, Activity activity, bool afk, int shard_id) {
	value presence;

	presence[U("op")] = value(3);

	presence[U("d")][U("since")] = value();
	if (activity.type == ActivityTypes::NoActivity)
		presence[U("d")][U("game")] = value();
	else
		presence[U("d")][U("game")] = activity.to_json();

	presence[U("d")][U("status")] = value(conversions::to_string_t(status));
	presence[U("d")][U("afk")] = value(afk);

	return pplx::create_task([this, presence, shard_id] {
		if (shard_id == -1) {
			for (unsigned int i = 0; i < _gateways.size(); i++) {
				_gateways[i]->send(presence).wait();
			}
		}
		else
			get_shard(shard_id)->send(presence).wait();
	});
}

DiscordCPP::MainGateway * DiscordCPP::Discord::get_shard(unsigned int shard_id) {
	for (unsigned int i = 0; i < _gateways.size(); i++) {
		if (_gateways[i]->get_shard_id() == shard_id)
			return _gateways[i];
	}

	throw ClientException("shard with id " + to_string(shard_id) + " not found");

	return NULL;
}

pplx::task<void> DiscordCPP::Discord::connect() {
	return pplx::create_task([this] {
		log.info("connecting to websocket: " + string(GATEWAY_URL));

		for (unsigned int i = 0; i < _gateways.size(); i++) {
			_gateways[i]->connect(GATEWAY_URL);

			waitFor(chrono::milliseconds(10000)).wait();
		}
	});
}

void DiscordCPP::Discord::on_websocket_incoming_message(value payload) {
	int op = payload.at(U("op")).as_integer();

	switch (op) {
		case 0:
			handle_raw_event(conversions::to_utf8string(payload.at(U("t")).as_string()), payload.at(U("d")));
			break;
		case 1:
		case 7:
		case 9:
		case 10:
		case 11:
			//already handled by MainGateway
			break;
		default:
			log.debug(conversions::to_utf8string(payload.serialize()));

			log.warning("ignoring payload with op-code: " + to_string(op));
			break;
	}
}

pplx::task<void> DiscordCPP::Discord::handle_raw_event(string event_name, value data) {
	return pplx::create_task([this, event_name, data] {
		try {	//https://discordapp.com/developers/docs/topics/gateway#commands-and-events-gateway-events
			if (event_name == "READY") {
				_user = new User(data.at(U("user")), _token);

				//_guilds
				if (is_valid_field("guilds")) {
					web::json::array tmp = data.at(U("guilds")).as_array();
					for (unsigned int i = 0; i < tmp.size(); i++)
						_guilds.push_back(new Guild(this, tmp[i], _token));
				}

				pplx::create_task([this] {
					try {
						on_ready(User(*_user));
					}
					catch (const exception &e) {
						log.error("ignoring exception in on_ready: " + string(e.what()));
					}
				});
			}
			else if (event_name == "CHANNEL_CREATE") {
				if (is_valid_field("guild_id")) {
					string guild_id = conversions::to_utf8string(data.at(U("guild_id")).as_string());
					Channel *channel = Channel::from_json(this, data, _token);

					for (unsigned int i = 0; i < _guilds.size(); i++) {
						if (_guilds[i]->id == guild_id) {
							_guilds[i]->channels.push_back(channel);

							break;
						}
					}
				}
			}
			else if (event_name == "CHANNEL_UPDATE") {
				if (is_valid_field("guild_id")) {
					string guild_id = conversions::to_utf8string(data.at(U("guild_id")).as_string());
					Channel *channel = Channel::from_json(this, data, _token);

					for (unsigned int i = 0; i < _guilds.size(); i++) {
						if (_guilds[i]->id == guild_id) {
							for (unsigned int j = 0; j < _guilds[i]->channels.size(); j++) {
								if (_guilds[i]->channels[j]->id == channel->id) {
									_guilds[i]->channels.erase(_guilds[i]->channels.begin() + j);
									_guilds[i]->channels.push_back(channel);
									break;
								}
							}

							break;
						}
					}
				}
			}
			else if (event_name == "CHANNEL_DELETE") {
				if (is_valid_field("guild_id")) {
					string guild_id = conversions::to_utf8string(data.at(U("guild_id")).as_string());
					string channel_id = conversions::to_utf8string(data.at(U("id")).as_string());

					for (unsigned int i = 0; i < _guilds.size(); i++) {
						if (_guilds[i]->id == guild_id) {
							for (unsigned int j = 0; j < _guilds[i]->channels.size(); j++) {
								if (_guilds[i]->channels[j]->id == channel_id) {
									_guilds[i]->channels.erase(_guilds[i]->channels.begin() + j);
									break;
								}
							}

							break;
						}
					}
				}
			}
			else if (event_name == "CHANNEL_PINS_UPDATE") {

			}
			else if (event_name == "MESSAGE_CREATE") {
				pplx::create_task([this, data] {
					try {
						on_message(Message(data, _token));
					}
					catch (const std::exception &e) {
						log.error("ignoring exception in on_message: " + string(e.what()));
					}
				});
			}
			else if (event_name == "GUILD_CREATE") {
				Guild *tmp_guild = new Guild(this, data, _token);

				for (unsigned int i = 0; i < _guilds.size(); i++) {
					if (tmp_guild->id == _guilds[i]->id) {
						_guilds[i] = tmp_guild;
						log.debug("Updated guild data");
						return;
					}
				}

				_guilds.push_back(tmp_guild);
				log.debug("data of new guild added");
			}
			else if (event_name == "VOICE_STATE_UPDATE") {
				if (_user->id != conversions::to_utf8string(data.at(U("user_id")).as_string()))
					return;

				VoiceState *voice_state = NULL;
				for (unsigned int i = 0; i < _voice_states.size(); i++) {
					if (_voice_states[i]->guild_id == data.at(U("guild_id")).as_string()) {
						voice_state = _voice_states[i];
						break;
					}
				}

				if (voice_state == NULL) {
					voice_state = new VoiceState();
					voice_state->guild_id = data.at(U("guild_id")).as_string();

					_voice_states.push_back(voice_state);
				}

				if (data.at(U("channel_id")).is_null()) {
					voice_state->channel_id = U("");
					voice_state->session_id = U("");
					voice_state->endpoint = U("");
					voice_state->voice_token = U("");
				}
				else {
					voice_state->channel_id = data.at(U("channel_id")).as_string();
					voice_state->session_id = data.at(U("session_id")).as_string();
				}
			}
			else if (event_name == "VOICE_SERVER_UPDATE") {
				VoiceState *voice_state = NULL;
				for (unsigned int i = 0; i < _voice_states.size(); i++) {
					if (_voice_states[i]->guild_id == data.at(U("guild_id")).as_string()) {
						voice_state = _voice_states[i];
						break;
					}
				}

				if (voice_state == NULL) {
					voice_state = new VoiceState();
					voice_state->guild_id = data.at(U("guild_id")).as_string();

					_voice_states.push_back(voice_state);
				}

				voice_state->endpoint = conversions::to_string_t("wss://") + data.at(U("endpoint")).as_string();
				voice_state->endpoint = voice_state->endpoint.substr(0, voice_state->endpoint.length() - 3) + conversions::to_string_t("?v=3");
				voice_state->voice_token = data.at(U("token")).as_string();
			}
			else {
				log.warning("ignoring event: " + event_name);
				//log.debug(conversions::to_utf8string(data.serialize()));
			}
		}
		catch (exception &e) {
			log.error("error while handling event " + event_name + ": " + e.what());
		}
	});
}
