#pragma once
#include <string>
#include <variant>
#include <vector>

#include "Channel.h"
#include "static.h"

namespace DiscordCPP {

class ApplicationCommandOption;
class ApplicationCommandSubcommand;
class ApplicationCommandChannelOption;
class ApplicationCommandValueOption;
class ApplicationCommandIntegerOption;
class ApplicationCommandNumberOption;

using ApplicationCommandOptionVariant = std::variant<ApplicationCommandSubcommand,
                                                     ApplicationCommandChannelOption,
                                                     ApplicationCommandValueOption,
                                                     ApplicationCommandIntegerOption,
                                                     ApplicationCommandNumberOption,
                                                     ApplicationCommandOption>;

class ApplicationCommandOption {
   public:
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
    };

   private:
    /// Type of the option.
    Type type;
    /// The option's name.
    std::string name;
    // name_localizations
    /// The option's description.
    std::string description;
    // description_localizations
    /// If the parameter is required. Default is false.
    bool required = false;

   public:
    DLL_EXPORT ApplicationCommandOption() = default;
    DLL_EXPORT explicit ApplicationCommandOption(const json& data);

    DLL_EXPORT json to_json();

    DLL_EXPORT Type get_type() { return type; }
    DLL_EXPORT std::string get_name() { return name; }
    DLL_EXPORT std::string get_description() { return description; }
    DLL_EXPORT bool is_required() { return required; }

    DLL_EXPORT void set_type(Type type) { this->type = type; }
    DLL_EXPORT void set_name(const std::string& name) { this->name = name; }
    DLL_EXPORT void set_description(const std::string& description) { this->description = description; }
    DLL_EXPORT void set_required(bool required) { this->required = required; }
};

class ApplicationCommandSubcommand : public ApplicationCommandOption {
   private:
    /// The parameters of this subcommand.
    std::vector<ApplicationCommandOptionVariant> options;

   public:
    DLL_EXPORT ApplicationCommandSubcommand();
    DLL_EXPORT explicit ApplicationCommandSubcommand(const json& data);

    DLL_EXPORT json to_json();

    DLL_EXPORT std::vector<ApplicationCommandOptionVariant> get_options() { return options; }

    DLL_EXPORT void add_option(const ApplicationCommandOptionVariant& option);
};

class ApplicationCommandChannelOption : public ApplicationCommandOption {
   private:
    /// The selectable channels will be restricted to these type.
    std::vector<Channel::Type> channel_types;

   public:
    DLL_EXPORT ApplicationCommandChannelOption();
    DLL_EXPORT explicit ApplicationCommandChannelOption(const json& data);

    DLL_EXPORT json to_json();

    DLL_EXPORT std::vector<Channel::Type> get_channel_types() { return channel_types; }

    DLL_EXPORT void add_channel_type(Channel::Type channel_type) { channel_types.push_back(channel_type); }
};

class ApplicationCommandValueOption : public ApplicationCommandOption {
   private:
    // choices
    /// If autocomplete is enabled. Only for STRING, INTEGER and NUMBER options.
    bool autocomplete = false;

   public:
    DLL_EXPORT ApplicationCommandValueOption();
    DLL_EXPORT explicit ApplicationCommandValueOption(const json& data);

    DLL_EXPORT json to_json();

    DLL_EXPORT bool is_autocomplete() { return autocomplete; }

    DLL_EXPORT void set_autocomplete(bool autocomplete) { this->autocomplete = autocomplete; }
};

class ApplicationCommandIntegerOption : public ApplicationCommandValueOption {
   private:
    /// The minumum allowed value.
    int min_value = 0;
    /// The maximum allowed value.
    int max_value = 0;

   public:
    DLL_EXPORT ApplicationCommandIntegerOption();
    DLL_EXPORT explicit ApplicationCommandIntegerOption(const json& data);

    DLL_EXPORT json to_json();

    DLL_EXPORT int get_min_value() { return min_value; }
    DLL_EXPORT int get_max_value() { return max_value; }

    DLL_EXPORT void set_min_value(int min_value) { this->min_value = min_value; }
    DLL_EXPORT void set_max_value(int max_value) { this->max_value = max_value; }
};

class ApplicationCommandNumberOption : public ApplicationCommandValueOption {
   private:
    /// The minumum allowed value.
    double min_value = 0;
    /// The maximum allowed value.
    double max_value = 0;

   public:
    DLL_EXPORT ApplicationCommandNumberOption();
    DLL_EXPORT explicit ApplicationCommandNumberOption(const json& data);

    DLL_EXPORT json to_json();

    DLL_EXPORT double get_min_value() { return min_value; }
    DLL_EXPORT double get_max_value() { return max_value; }

    DLL_EXPORT void set_min_value(double min_value) { this->min_value = min_value; }
    DLL_EXPORT void set_max_value(double max_value) { this->max_value = max_value; }
};

class ApplicationCommandOptionHelper {
   public:
    DLL_EXPORT static ApplicationCommandOptionVariant application_command_option_from_json(const json& data);
};

}  // namespace DiscordCPP
