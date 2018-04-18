#include "Discord.h"
#include <Windows.h>
#include <conio.h>
#include <stdlib.h>

using namespace DiscordCPP;

class myClient : public Discord {
public:
	void on_ready(User user) {
		log.info("logged in as: " + user.username);
	}

	void on_message(Message message) {
		log.info(message.author->username + "\" sent " + message.content + "\" in channel: " + message.channel->name + " (id: " + message.channel->id + ", type: " + to_string(message.channel->type) + ").");
		
		if (message.content.compare("?hello") == 0) {
			Message msg = send_message(message.channel, "Hello world!");
			log.info("message sent (id: " + msg.channel->id + ")");
		}
		else if (message.content == "?guild") {
			if (message.channel->type == ChannelType::GUILD_TEXT) {
				send_message(message.channel, ((GuildChannel *)message.channel)->guild->name);
			}
			else {
				send_message(message.channel, "This is not a guild channel.");
			}
		}
	}

	myClient(string token) : Discord(token) {};
};

int main() {
	char *token;
	size_t len;
	if (_dupenv_s(&token, &len, "DISCORD_TEST_TOKEN")) {
		cerr << "Error" << endl;
		return -1;
	}

	myClient *client = new myClient(token);
	free(token);

	client->log.set_log_level(DEBUG);

	while (_getch() != 'q') {
		Sleep(10);
	}

	delete client;

	return 0;
}