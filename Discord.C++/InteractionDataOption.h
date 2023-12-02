#pragma once
#include <variant>
#include <vector>

#include "ApplicationCommandOption.h"
#include "Threadpool.h"
#include "static.h"

namespace DiscordCPP {

class InteractionDataOption;
class InteractionDataStringOption;
class InteractionDataIntegerOption;
class InteractionDataNumberOption;
class InteractionDataBooleanOption;
class InteractionDataSubcommandOption;

using InteractionDataOptionVariant = std::variant<InteractionDataStringOption,
                                                  InteractionDataIntegerOption,
                                                  InteractionDataNumberOption,
                                                  InteractionDataBooleanOption,
                                                  InteractionDataSubcommandOption,
                                                  InteractionDataOption>;

class InteractionDataOption {
   public:
    /// Name of the parameter.
    std::string name;
    /// Type of the option.
    ApplicationCommandOption::Type type;

    DLL_EXPORT explicit InteractionDataOption(const json& data);
};

class InteractionDataStringOption : public InteractionDataOption {
   public:
    /// Value of the option.
    std::string value;

    DLL_EXPORT explicit InteractionDataStringOption(const json& data);
};

class InteractionDataIntegerOption : public InteractionDataOption {
   public:
    /// Value of the option.
    int value;

    DLL_EXPORT explicit InteractionDataIntegerOption(const json& data);
};

class InteractionDataNumberOption : public InteractionDataOption {
   public:
    /// Value of the option.
    double value;

    DLL_EXPORT explicit InteractionDataNumberOption(const json& data);
};

class InteractionDataBooleanOption : public InteractionDataOption {
   public:
    /// Value of the option.
    bool value;

    DLL_EXPORT explicit InteractionDataBooleanOption(const json& data);
};

class InteractionDataSubcommandOption : public InteractionDataOption {
   public:
    /// array of InteractionDataOption
    std::vector<InteractionDataOptionVariant> options;

    DLL_EXPORT explicit InteractionDataSubcommandOption(const json& data);
};

class InteractionDataOptionHelper {
   public:
    DLL_EXPORT static InteractionDataOptionVariant interaction_data_option_from_json(const json& data);
    DLL_EXPORT static std::string get_interaction_data_option_name(const InteractionDataOptionVariant& variant);
    DLL_EXPORT static ApplicationCommandOption::Type get_interaction_data_option_type(const InteractionDataOptionVariant& variant);
};

}  // namespace DiscordCPP