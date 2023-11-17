#pragma once
#include <vector>

#include "Channel.h"
#include "static.h"

namespace DiscordCPP {
class ApplicationCommandOption {
   public:
    /// Type of the option.
    enum Type {
        SUB_COMMAND = 1,
        SUB_COMMAND_GROUP = 2,
        STRING = 3,
        INTEGER = 4,
        BOOLEAN = 5,
        USER = 6,
        CHANNEL = 7,
        ROLE = 8,
        MENTIONABLE = 9,
        NUMBER = 10,
        ATTACHMENT = 11
    } type;
    /// The option's name.
    std::string name;
    // name_localizations
    /// The option's description.
    std::string description;
    // description_localizations
    /// If the parameter is required. Default is false.
    bool required = false;

    DLL_EXPORT ApplicationCommandOption() = default;
    DLL_EXPORT explicit ApplicationCommandOption(const json& data);
    DLL_EXPORT static ApplicationCommandOption* from_json(const json& data);

    DLL_EXPORT json to_json();

    DLL_EXPORT ApplicationCommandOption* copy();
};

class ApplicationCommandSubcommand : public ApplicationCommandOption {
   public:
    /// The parameters of this subcommand.
    std::vector<ApplicationCommandOption*> options;

    DLL_EXPORT ApplicationCommandSubcommand();
    DLL_EXPORT explicit ApplicationCommandSubcommand(const json& data);
    DLL_EXPORT ApplicationCommandSubcommand(const ApplicationCommandSubcommand& other);
    DLL_EXPORT ~ApplicationCommandSubcommand();

    DLL_EXPORT json to_json();
};

class ApplicationCommandChannelOption : public ApplicationCommandOption {
   public:
    /// The selectable channels will be restricted to these type.
    std::vector<Channel::Type> channel_types;

    DLL_EXPORT ApplicationCommandChannelOption();
    DLL_EXPORT explicit ApplicationCommandChannelOption(const json& data);

    DLL_EXPORT json to_json();
};

class ApplicationCommandValueOption : public ApplicationCommandOption {
   public:
    // choices
    /// If autocomplete is enabled. Only for STRING, INTEGER and NUMBER options.
    bool autocomplete = false;

    DLL_EXPORT ApplicationCommandValueOption();
    DLL_EXPORT explicit ApplicationCommandValueOption(const json& data);

    DLL_EXPORT json to_json();
};

class ApplicationCommandIntegerOption : public ApplicationCommandValueOption {
   public:
    /// The minumum allowed value.
    int min_value = 0;
    /// The maximum allowed value.
    int max_value = 0;

    DLL_EXPORT ApplicationCommandIntegerOption();
    DLL_EXPORT explicit ApplicationCommandIntegerOption(const json& data);

    DLL_EXPORT json to_json();
};

class ApplicationCommandNumberOption : public ApplicationCommandValueOption {
   public:
    /// The minumum allowed value.
    double min_value = 0;
    /// The maximum allowed value.
    double max_value = 0;

    DLL_EXPORT ApplicationCommandNumberOption();
    DLL_EXPORT explicit ApplicationCommandNumberOption(const json& data);

    DLL_EXPORT json to_json();
};
}  // namespace DiscordCPP