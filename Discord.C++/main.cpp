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
		if (message.content.compare("?hello") == 0) {
			log.info("hello world!");
			send_message(message.channel_id, "Hello world!");
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

	myClient client = myClient(string(token));
	free(token);

	client.log.set_log_level(DEBUG);

	while (_getch() != 'q') {
		Sleep(10);
	}

	return 0;
}