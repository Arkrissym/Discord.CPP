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
			Sleep(1000);
			msg.edit("Edited message");
		}
		else if (message.content == "?msgdel") {
			Message msg = message.channel->send("Message will be deleted in 5 seconds.");
			Sleep(5000);
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