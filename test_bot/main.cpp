#include <chrono>
#include <iostream>
#include <thread>
#ifndef _WIN32
#include <stdlib.h>
#endif
#include <boost/process.hpp>

#include "Discord.h"

/*	Compile with smth. like this:
    g++ -Wall -o test main.cpp -ldiscord_cpp -lcpprest -lpthread -lssl -lcrypto
*/

using namespace DiscordCPP;
using namespace std;

class Client : public Discord {
   private:
    Channel* findChannel(const string& name, const string& guild_id) {
        for (unsigned int i = 0; i < _guilds.size(); i++) {
            if (guild_id == _guilds[i]->id) {
                for (unsigned int j = 0; j < _guilds[i]->channels.size(); j++) {
                    if (_guilds[i]->channels[j]->name.compare(name) == 0) {
                        return _guilds[i]->channels[j];
                    }
                }
            }
        }
        return NULL;
    }
    void play(VoiceChannel* channel, AudioSource* source) {
        VoiceClient vc = channel->connect();

        try {
            vc.play(source).wait();
        } catch (const OpusError& e) {
            log.error("Opus error: " + string(e.what()) +
                      " (code: " + to_string(e.get_error_code()) + ")");
        } catch (const ClientException& e) {
            log.error("ClientException in vc->play: " + string(e.what()));
        }
    }

    void on_ready(User user) override {
        log.info("logged in as: " + user.username);

        this->update_presence(DiscordStatus::Online,
                              Activity("test", ActivityTypes::Game));
    }

    void on_user_ban(User user, Guild guild) override {
        log.info("User " + string(user) + " has been banned from Guild " +
                 string(guild));
    }

    void on_user_unban(User user, Guild guild) override {
        log.info("User " + string(user) + " has been unbanned from Guild " +
                 string(guild));
    }

    void on_user_join(Member member, Guild guild) override {
        log.info("Member " + string(member) + " joined Guild " + string(guild));
    }

    void on_user_remove(User user, Guild guild) override {
        log.info("Member " + string(user) + " left Guild " + string(guild));
    }

    void on_typing_start(User user, TextChannel channel, unsigned int) override {
        log.debug(user.username + " started typing in " + channel.name);
    }

    void on_message(Message message) override {
        log.debug(message.author->username + " sent \"" + message.content +
                  "\" in channel: " + message.channel->name +
                  " (id: " + message.channel->id +
                  ", type: " + to_string(message.channel->type) + ").");

        if (message.content.compare("?hello") == 0) {
            Message msg = message.channel->send("Hello World!");
            log.info("message sent (id: " + msg.channel->id + ")");
        } else if (message.content == "?guild") {
            if (message.channel->type == ChannelType::GUILD_TEXT) {
                message.channel->send(*((GuildChannel*)message.channel)->guild);
            } else {
                message.channel->send("This is not a guild channel.");
            }
        } else if (message.content == "?msgedit") {
            Message msg = message.channel->send("New message");
            this_thread::sleep_for(chrono::seconds(1));
            msg.edit("Edited message");
        } else if (message.content == "?msgdel") {
            Message msg =
                message.channel->send("Message will be deleted in 5 seconds.");
            this_thread::sleep_for(chrono::seconds(5));
            msg.delete_msg();
            message.delete_msg();
        } else if (message.content == "?chdel") {
            message.channel->delete_channel();
        } else if (message.content == "?embed") {
            Embed embed = Embed("Embed", "description");

            embed.set_color(0x00ff00);
            embed.set_author("TestBot", "https://github.com",
                             "https://cdn.pixabay.com/photo/2016/02/22/00/25/"
                             "robot-1214536_960_720.png");
            embed.set_footer("Footer",
                             "https://cdn.pixabay.com/photo/2016/02/22/00/25/"
                             "robot-1214536_960_720.png");
            embed.set_thumbnail(
                "https://cdn.pixabay.com/photo/2016/02/22/00/25/"
                "robot-1214536_960_720.png");
            embed.set_image(
                "https://cdn.pixabay.com/photo/2016/02/22/00/25/"
                "robot-1214536_960_720.png");

            for (int i = 0; i < 4; i++) {
                embed.add_field("Field", to_string(i));
            }
            for (int i = 4; i < 7; i++) {
                embed.add_field("Field", to_string(i), false);
            }

            message.channel->send(embed);
        } else if (message.content == "?dm") {
            message.author->send("This is a direct text message.");
            message.author->send(Embed("Embed", "text"));
        } else if (message.content.compare(0, 7, "?clear ") == 0) {
            string tmp = message.content.substr(7);
            vector<shared_ptr<Message>> messages =
                message.channel->history(atoi(tmp.c_str()) + 1);

            log.info("num messages: " + to_string(messages.size()));
            message.channel->delete_messages(messages);
        } else if (message.content.compare(0, 9, "?playwav ") == 0) {
            if (message.channel->type != ChannelType::GUILD_TEXT) {
                return;
            }

            FileAudioSource* source = new FileAudioSource("test.wav");
            string channel_name = message.content.substr(9);

            play((VoiceChannel*)findChannel(
                     channel_name, ((GuildChannel*)message.channel)->guild->id),
                 source);

            delete source;
        } else if (message.content.compare(0, 12, "?playffmpeg ") == 0) {
            if (message.channel->type != ChannelType::GUILD_TEXT) {
                return;
            }

            FFmpegAudioSource* source = new FFmpegAudioSource("test.mp3");
            string channel_name = message.content.substr(12);

            play((VoiceChannel*)findChannel(
                     channel_name, ((GuildChannel*)message.channel)->guild->id),
                 source);

            delete source;
        } else if (message.content.compare(0, 6, "?play ") == 0) {
            stringstream argumentstream(message.content.substr(6));
            string channel_name;
            string link;
            getline(argumentstream, link, ' ');
            getline(argumentstream, channel_name);

            boost::filesystem::path p =
                boost::process::search_path("youtube-dl");
            boost::process::ipstream input;
            boost::process::system(p, "-g", "-x", "--no-playlist", link,
                                   boost::process::std_out > input,
                                   boost::process::std_err > stderr);

            string url;
            getline(input, url);

            FFmpegAudioSource* source = new FFmpegAudioSource(
                url,
                "-reconnect 1 -reconnect_streamed 1 -reconnect_delay_max 5",
                "-af "
                "loudnorm=i=-23.0:lra=7.0:tp=-2.0:offset=0.0:measured_i=-9.11:"
                "measured_lra=3.5:measured_tp=-0.03:measured_thresh=-19.18:"
                "linear=true[norm0]");
            play((VoiceChannel*)findChannel(
                     channel_name, ((GuildChannel*)message.channel)->guild->id),
                 source);

            delete source;
        } else if (message.content == "?leave_guild") {
            if (message.channel->type == ChannelType::GUILD_TEXT) {
                ((GuildChannel*)message.channel)->guild->leave();
            } else {
                message.channel->send("This is not a guild channel.");
            }
        } else if (message.content == "?delete_guild") {
            if (message.channel->type == ChannelType::GUILD_TEXT) {
                ((GuildChannel*)message.channel)->guild->delete_guild();
            } else {
                message.channel->send("This is not a guild channel.");
            }
        } else if (message.content.compare(0, 6, "?kick ") == 0) {
            if (message.channel->type == ChannelType::GUILD_TEXT) {
                Guild* guild = NULL;

                for (unsigned int i = 0; i < _guilds.size(); i++) {
                    if (((GuildChannel*)message.channel)->guild->id ==
                        _guilds[i]->id) {
                        guild = _guilds[i];
                        break;
                    }
                }

                string user_name = message.content.substr(6);
                Member* user = NULL;

                for (unsigned int i = 0; i < guild->members.size(); i++) {
                    log.debug(string(*guild->members[i]));
                    if (string(*guild->members[i]) == user_name) {
                        user = guild->members[i];
                        break;
                    }
                }

                if (user == NULL) {
                    message.channel->send("User " + user_name + " not found.");
                } else {
                    guild->kick(*user);
                }
            } else {
                message.channel->send("This is not a guild channel.");
            }
        } else if (message.content.compare(0, 5, "?ban ") == 0) {
            if (message.channel->type == ChannelType::GUILD_TEXT) {
                Guild* guild = NULL;

                for (unsigned int i = 0; i < _guilds.size(); i++) {
                    if (((GuildChannel*)message.channel)->guild->id ==
                        _guilds[i]->id) {
                        guild = _guilds[i];
                        break;
                    }
                }

                string user_name = message.content.substr(5);
                Member* user_ptr = NULL;

                for (unsigned int i = 0; i < guild->members.size(); i++) {
                    if (string(*guild->members[i]) == user_name) {
                        user_ptr = guild->members[i];
                        break;
                    }
                }

                if (user_ptr == NULL) {
                    message.channel->send("User " + user_name + " not found.");
                } else {
                    User user = User(*user_ptr);

                    guild->ban(user, "testing the library", 1);

                    this_thread::sleep_for(chrono::seconds(10));

                    guild->unban(user);
                }
            } else {
                message.channel->send("This is not a guild channel.");
            }
        }
    }

   public:
    Client(const string& token, const Intents& intents, unsigned int num_shards = 0) : Discord(token, intents, num_shards){};
};

int main() {
    char* token;
#ifdef _WIN32
    size_t len;
    if (_dupenv_s(&token, &len, "DISCORD_TEST_TOKEN")) {
        cerr << "Error" << endl;
        return -1;
    }
#else
    token = getenv("DISCORD_TEST_TOKEN");
#endif

    Intents intents = Intents::Default();
    intents.add(Intents::MEMBERS);
    Client client = Client(token, intents);

    client.log.set_log_level(Debug);

    client.start();

    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
