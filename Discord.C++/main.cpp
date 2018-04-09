#include "Discord.h"
#include <Windows.h>
#include <conio.h>

#define TOKEN		"NDMxODIzMjMwNDU4Nzg5ODg4.DakWXQ.hbzNlbufXINBw8kRGJRigxG2LeU"

using namespace DiscordCPP;

class myClient : public Discord {
public:
	void on_ready(User user) {
		log.info("logged in as: " + user.username);
	};

	myClient(string token) : Discord(token) {};
};

int main() {
	myClient client = myClient(TOKEN);

	client.log.set_log_level(DEBUG);

	while (_getch() != 'q') {
		Sleep(10);
	}

	return 0;
}