#include <chrono>
#include <thread>
#include <vector>

#ifndef _WIN32
#include <cstdlib>
#endif
#include <boost/process.hpp>

#include "Discord.h"

using namespace DiscordCPP;
using namespace std;

class Client : public Discord {
   private:
    ChannelVariant findChannel(const string& name, const string& guild_id) {
        for (auto& _guild : _guilds) {
            if (guild_id == _guild->get_id()) {
                auto channels = _guild->get_channels();
                for (auto& channel : channels) {
                    if (std::visit([](auto& c) { return c.get_name(); }, channel).compare(name) == 0) {
                        return channel;
                    }
                }
            }
        }
        return {};
    }
    void play(ChannelVariant channel, AudioSource* source) {
        std::shared_ptr<VoiceClient> vc = std::get<VoiceChannel>(channel).connect();

        try {
            vc->play(source).get();
            vc->disconnect().get();
        } catch (const OpusError& e) {
            log.error("Opus error: " + string(e.what()) +
                      " (code: " + to_string(e.get_error_code()) + ")");
        } catch (const ClientException& e) {
            log.error("ClientException in vc->play: " + string(e.what()));
        }
    }

    void on_ready(User user) override {
        log.info("logged in as: " + user.get_username());

        auto old_commands = get_application_commands();
        for (auto command : old_commands) {
            command.delete_command();
        }

        ApplicationCommand ping = ApplicationCommand();
        ping.set_name("ping");
        ping.set_description("Ping the bot");
        ping.set_type(ApplicationCommand::Type::CHAT_INPUT);
        create_application_command(ping);

        ApplicationCommand update = ApplicationCommand();
        update.set_name("update");
        update.set_description("Responds with a message and updates it after 5 seconds");
        update.set_type(ApplicationCommand::Type::CHAT_INPUT);
        create_application_command(update);

        ApplicationCommand msg = ApplicationCommand();
        msg.set_name("msg");
        msg.set_description("Test messages");
        msg.set_type(ApplicationCommand::Type::CHAT_INPUT);
        ApplicationCommandSubcommand text;
        text.set_name("text");
        text.set_description("Text message");
        text.set_type(ApplicationCommandOption::Type::SUB_COMMAND);
        msg.add_option(text);
        ApplicationCommandSubcommand embed;
        embed.set_name("embed");
        embed.set_description("Embed message");
        embed.set_type(ApplicationCommandOption::Type::SUB_COMMAND);
        msg.add_option(embed);
        create_application_command(msg);

        ApplicationCommand echo = ApplicationCommand();
        echo.set_name("echo");
        echo.set_type(ApplicationCommand::Type::MESSAGE);
        create_application_command(echo);

        ApplicationCommand say = ApplicationCommand();
        say.set_name("say");
        say.set_description("Reply with given message");
        say.set_type(ApplicationCommand::Type::CHAT_INPUT);
        ApplicationCommandValueOption m;
        m.set_name("message");
        m.set_description("Message to reply");
        m.set_required(true);
        m.set_type(ApplicationCommandOption::Type::STRING);
        say.add_option(m);
        create_application_command(say);

        ApplicationCommand react = ApplicationCommand();
        react.set_name("react");
        react.set_type(ApplicationCommand::Type::MESSAGE);
        create_application_command(react);

        update_presence(DiscordStatus::Online, Activity("test", Activity::Type::Game));
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
        log.info(user.get_username() + " started typing in " + channel.get_name());
    }

    void on_message(Message message) override {
        log.info(message.get_author().value().get_username() + " sent \"" + message.get_content() +
                 "\" in channel: " + message.get_channel().get_name() +
                 " (id: " + message.get_channel_id() +
                 ", type: " + to_string(message.get_channel().get_type()) + ").");

        if (message.get_content().compare("?hello") == 0) {
            Message msg = message.reply("Hello World!");
            log.info("message sent (id: " + msg.get_id() + ")");
        } else if (message.get_content() == "?guild") {
            if (message.get_guild().has_value()) {
                message.reply(message.get_guild().value().get_name());
            } else {
                message.reply("This is not a guild channel.");
            }
        } else if (message.get_content() == "?msgedit") {
            Message msg = message.get_channel().send("New message");
            this_thread::sleep_for(chrono::seconds(1));
            msg.edit("Edited message");
        } else if (message.get_content() == "?msgdel") {
            Message msg = message.get_channel().send("Message will be deleted in 5 seconds.");
            this_thread::sleep_for(chrono::seconds(5));
            msg.delete_msg();
            message.delete_msg();
        } else if (message.get_content() == "?chdel") {
            message.get_channel().delete_channel();
        } else if (message.get_content() == "?embed") {
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

            message.reply(embed);
        } else if (message.get_content() == "?dm") {
            message.get_author().value().send("This is a direct text message.");
            message.get_author().value().send(Embed("Embed", "text"));
        } else if (message.get_content().compare(0, 7, "?clear ") == 0) {
            string tmp = message.get_content().substr(7);
            vector<shared_ptr<Message>> messages = message.get_channel().history(atoi(tmp.c_str()) + 1);

            log.info("num messages: " + to_string(messages.size()));
            message.get_channel().delete_messages(messages);
        } else if (message.get_content().compare(0, 9, "?playwav ") == 0) {
            if (!message.get_guild_id().has_value()) {
                return;
            }

            FileAudioSource* source = new FileAudioSource("test.wav");
            string channel_name = message.get_content().substr(9);

            play(findChannel(channel_name, message.get_guild_id().value()), source);

            delete source;
        } else if (message.get_content().compare(0, 12, "?playffmpeg ") == 0) {
            if (!message.get_guild_id().has_value()) {
                return;
            }

            FFmpegAudioSource* source = new FFmpegAudioSource("test.mp3");
            string channel_name = message.get_content().substr(12);

            play(findChannel(channel_name, message.get_guild_id().value()), source);

            delete source;
        } else if (message.get_content().compare(0, 6, "?play ") == 0) {
            if (!message.get_guild_id().has_value()) {
                return;
            }

            stringstream argumentstream(message.get_content().substr(6));
            string channel_name;
            string link;
            getline(argumentstream, link, ' ');
            getline(argumentstream, channel_name);

            boost::filesystem::path p =
                boost::process::search_path("yt-dlp");
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
            play(findChannel(channel_name, message.get_guild_id().value()), source);

            delete source;
        } else if (message.get_content() == "?leave_guild") {
            if (message.get_guild().has_value()) {
                message.get_guild().value().leave();
            } else {
                message.reply("This is not a guild channel.");
            }
        } else if (message.get_content() == "?delete_guild") {
            if (message.get_guild().has_value()) {
                message.get_guild().value().delete_guild();
            } else {
                message.reply("This is not a guild channel.");
            }
        } else if (message.get_content().compare(0, 6, "?kick ") == 0) {
            if (message.get_guild_id().has_value()) {
                Guild* guild = NULL;

                for (unsigned int i = 0; i < _guilds.size(); i++) {
                    if (message.get_guild_id().value() ==
                        _guilds[i]->get_id()) {
                        guild = _guilds[i];
                        break;
                    }
                }

                string user_name = message.get_content().substr(6);
                std::optional<Member> user;
                auto members = guild->get_members();
                for (auto& member : members) {
                    log.debug(string(member));
                    if (string(member) == user_name) {
                        user = member;
                        break;
                    }
                }

                if (!user.has_value()) {
                    message.reply("User " + user_name + " not found.");
                } else {
                    guild->kick(*user);
                }
            } else {
                message.reply("This is not a guild channel.");
            }
        } else if (message.get_content().compare(0, 5, "?ban ") == 0) {
            if (message.get_guild().has_value()) {
                Guild* guild = nullptr;

                for (auto& _guild : _guilds) {
                    if (message.get_guild().value().get_id() == _guild->get_id()) {
                        guild = _guild;
                        break;
                    }
                }

                string user_name = message.get_content().substr(5);
                std::optional<Member> user;
                auto members = guild->get_members();

                for (auto& member : members) {
                    if (string(member) == user_name) {
                        user = member;
                        break;
                    }
                }

                if (!user.has_value()) {
                    message.reply("User " + user_name + " not found.");
                } else {
                    guild->ban(user.value(), "testing the library", 1);

                    this_thread::sleep_for(chrono::seconds(10));

                    guild->unban(user.value());
                }
            } else {
                message.reply("This is not a guild channel.");
            }
        }
    }

    void on_message_update(Message message) override {
        log.info(message.get_author().value().get_username() + " updated \"" + message.get_content() +
                 "\" in channel: " + message.get_channel().get_name() +
                 " (id: " + message.get_channel_id() +
                 ", type: " + to_string(message.get_channel().get_type()) + ").");
    }

    void on_message_delete(Message message) override {
        log.info("message " + message.get_id() +
                 " deleted in channel: " + message.get_channel().get_name() +
                 " (id: " + message.get_channel_id() +
                 ", type: " + to_string(message.get_channel().get_type()) + ").");
    }

    void on_message_delete_bulk(std::vector<Message> messages) override {
        std::string ids;
        for (Message message : messages) {
            if (ids.length() > 0) {
                ids.append(", ");
            }
            ids.append(message.get_id());
        }

        log.info("messages deleted: " + ids +
                 " in channel: " + messages.front().get_channel().get_name());
    }

    void on_message_reaction(Reaction reaction) override {
        log.info(reaction.get_user().get_username() +
                 " reacted with " + reaction.get_emoji().get_name().value_or("") +
                 " (id: " + reaction.get_emoji().get_id().value_or("none") +
                 ") in channel " + reaction.get_channel().get_name() +
                 " (id: " + reaction.get_channel_id() + ")");
    }

    void on_message_reaction_delete(Reaction reaction) override {
        log.info(reaction.get_user().get_username() +
                 " deleted reaction " + reaction.get_emoji().get_name().value_or("") +
                 " (id: " + reaction.get_emoji().get_id().value_or("none") +
                 ") in channel " + reaction.get_channel().get_name() +
                 " (id: " + reaction.get_channel_id() + ")");
    }

    void on_message_reaction_delete_all(Message message) override {
        log.info("All reactions have been removed from message " + message.get_id() +
                 " in channel: " + message.get_channel_id());
    }

    void on_interaction(Interaction interaction) override {
        if (interaction.get_type() != Interaction::Type::APPLICATION_COMMAND) {
            return;
        }

        string name = interaction.get_data().value().get_name();
        log.info("interaction: " + name);

        if (name == "ping") {
            interaction.reply("pong");
        } else if (name == "update") {
            interaction.reply("Original reply");
            this_thread::sleep_for(chrono::seconds(5));
            interaction.update_reply("Updated reply");
        } else if (name == "echo") {
            auto data = interaction.get_data().value();
            if (data.get_resolved_data().has_value() && data.get_target_id().has_value()) {
                Message message = data.get_resolved_data().value().get_messages().at(data.get_target_id().value());
                interaction.reply(message.get_content());
            }
        } else if (name == "say") {
            std::string m = std::get<InteractionDataStringOption>(interaction.get_data().value().get_options().at(0)).value;
            interaction.reply(m);
        } else if (name == "msg") {
            auto options = interaction.get_data().value().get_options();
            for (auto& option : options) {
                log.debug(InteractionDataOptionHelper::get_interaction_data_option_name(option));
                log.debug(std::to_string(InteractionDataOptionHelper::get_interaction_data_option_type(option)));
                if (InteractionDataOptionHelper::get_interaction_data_option_name(option) == "text" && InteractionDataOptionHelper::get_interaction_data_option_type(option) == ApplicationCommandOption::Type::SUB_COMMAND) {
                    interaction.reply("Text reply");
                } else if (InteractionDataOptionHelper::get_interaction_data_option_name(option) == "embed" && InteractionDataOptionHelper::get_interaction_data_option_type(option) == ApplicationCommandOption::Type::SUB_COMMAND) {
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

                    interaction.reply(embed);

                } else {
                    log.info(InteractionDataOptionHelper::get_interaction_data_option_name(option) + " " + std::to_string(InteractionDataOptionHelper::get_interaction_data_option_type(option)));
                }
            }
        } else if (name == "react") {
            auto data = interaction.get_data().value();
            if (data.get_resolved_data().has_value() && data.get_target_id().has_value()) {
                Message message = data.get_resolved_data().value().get_messages().at(data.get_target_id().value());
                interaction.reply(":100:");

                Emoji thumbs_up = Emoji::by_name("👍");
                Emoji thumbs_down = Emoji::by_name("👎");
                Emoji e100 = Emoji::by_name("💯");

                message.add_reaction(thumbs_up);
                this_thread::sleep_for(chrono::seconds(1));
                message.add_reaction(thumbs_down);
                this_thread::sleep_for(chrono::seconds(1));
                message.add_reaction(e100);
                this_thread::sleep_for(chrono::seconds(1));

                message.remove_reaction(thumbs_down);
                this_thread::sleep_for(chrono::seconds(1));
                message.remove_user_reaction(thumbs_up, *_user);
                this_thread::sleep_for(chrono::seconds(1));
                message.remove_all_reactions(e100);
                this_thread::sleep_for(chrono::seconds(1));

                message.add_reaction(e100);
                this_thread::sleep_for(chrono::seconds(1));
                message.remove_all_reactions();
            }
        }
    }

   public:
    Client(const string& token, const Intents& intents, unsigned int num_shards = 0)
        : Discord(token, intents, num_shards) {};
};

int main() {
    char* token = nullptr;
#ifdef _WIN32
    size_t len;
    if (_dupenv_s(&token, &len, "DISCORD_TEST_TOKEN")) {
        cerr << "Error" << endl;
        return -1;
    }
#else
    token = getenv("DISCORD_TEST_TOKEN");
#endif

    Logger::register_thread(std::this_thread::get_id(), "main");
    Logger::set_log_level(Debug);

    Intents intents = Intents::All();
    intents.add(Intents::MEMBERS);
    intents.add(Intents::MESSAGE_CONTENT);
    Client client = Client(token, intents);

    client.start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
