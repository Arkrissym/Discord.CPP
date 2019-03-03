#ifdef _WIN32
#include "Discord.h"
#include <iostream>
#else
#include <discord_cpp/Discord.h>
#include <stdlib.h>
#endif
#include <chrono>
#include <thread>

using namespace DiscordCPP;
using namespace std;

class myClient : public Discord {
public:
	void on_ready(User user) {
		log.info("logged in as: " + user.username);

		this->update_presence(DiscordStatus::Online, Activity("test", ActivityTypes::Game));
	}

	void on_message(Message message) {
		//log.info(message.author->username + "\" sent " + message.content + "\" in channel: " + message.channel->name + " (id: " + message.channel->id + ", type: " + to_string(message.channel->type) + ").");
		
		if (message.content.compare("?hello") == 0) {
			Message msg = message.channel->send("Hello World!");
			log.info("message sent (id: " + msg.channel->id + ")");
		}
		else if (message.content == "?guild") {
			if (message.channel->type == ChannelType::GUILD_TEXT) {
				message.channel->send(*((GuildChannel *)message.channel)->guild);
			}
			else {
				message.channel->send("This is not a guild channel.");
			}
		}
		else if (message.content == "?msgedit") {
			Message msg = message.channel->send("New message");
			this_thread::sleep_for(chrono::seconds(1));
			msg.edit("Edited message");
		}
		else if (message.content == "?msgdel") {
			Message msg = message.channel->send("Message will be deleted in 5 seconds.");
			this_thread::sleep_for(chrono::seconds(5));
			msg.delete_msg();
			message.delete_msg();
		}
		else if (message.content == "?chdel") {
			message.channel->delete_channel();
		}
		else if (message.content == "?embed") {
			Embed embed = Embed("Embed", "description");

			embed.set_color(0x00ff00);
			embed.set_author("TestBot", "https://github.com", "https://cdn.pixabay.com/photo/2016/02/22/00/25/robot-1214536_960_720.png");
			embed.set_footer("Footer", "https://cdn.pixabay.com/photo/2016/02/22/00/25/robot-1214536_960_720.png");
			embed.set_thumbnail("https://cdn.pixabay.com/photo/2016/02/22/00/25/robot-1214536_960_720.png");
			embed.set_image("https://cdn.pixabay.com/photo/2016/02/22/00/25/robot-1214536_960_720.png");
			embed.set_provider("Provider", "https://github.com");

			for (int i = 0; i < 4; i++) {
				embed.add_field("Field", to_string(i));
			}
			for (int i = 4; i < 7; i++) {
				embed.add_field("Field", to_string(i), false);
			}
			
			message.channel->send(embed);
		}
		else if (message.content == "?dm") {
			message.author->send("This is a direct text message.");
			message.author->send(Embed("Embed", "text"));
		}
		else if (message.content.compare(0, 7, "?clear ") == 0) {
			string tmp = message.content.substr(7);
			vector<shared_ptr<Message>> messages = message.channel->history(atoi(tmp.c_str()) + 1);

			log.info("num messages: " + to_string(messages.size()));
			message.channel->delete_messages(messages);
		}
		else if (message.content.compare(0, 6, "?join ") == 0) {
			if (message.channel->type != ChannelType::GUILD_TEXT) {
				return;
			}

			string channel_name = message.content.substr(6);

			Guild *guild = NULL;

			for (unsigned int i = 0; i < _guilds.size(); i++) {
				if (((GuildChannel *)message.channel)->guild->id == _guilds[i]->id) {
					guild = _guilds[i];
					break;
				}
			}

			for (unsigned int i = 0; i < guild->channels.size(); i++) {
				if (guild->channels[i]->name.compare(channel_name) == 0) {
					VoiceClient *vc = ((VoiceChannel *)guild->channels[i])->connect();

					try {
						vc->play(&FileAudioSource("test.wav")).wait();
					}
					catch (const OpusError &e) {
						log.error("Opus error: " + string(e.what()) + " (code: " + to_string(e.get_error_code()) + ")");
					}
					catch (const ClientException &e) {
						log.error("ClientException in vc->play: " + string(e.what()));
					}
					vc->disconnect().wait();
					
					delete vc;

					return;
				}
			}

			message.channel->send("Channel not found");
		}
	}

	myClient(string token, unsigned int num_shards = 1) : Discord(token, num_shards) {};
};

int main() {
	char *token;
#ifdef _WIN32
	size_t len;
	if (_dupenv_s(&token, &len, "DISCORD_TEST_TOKEN")) {
		cerr << "Error" << endl;
		return -1;
	}
#else
	token = getenv("DISCORD_TEST_TOKEN");
#endif

	myClient *client = new myClient(token, 2);

#ifdef _WIN32
	free(token);
#endif

	client->log.set_log_level(DEBUG);

	while (1) {
		this_thread::sleep_for(chrono::seconds(1));
	}

	delete client;

	return 0;
}
