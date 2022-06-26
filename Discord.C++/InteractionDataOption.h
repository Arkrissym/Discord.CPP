#pragma once
#include <vector>
#include "ApplicationCommandOption.h"
#include "static.h"

namespace DiscordCPP {

class InteractionDataOption {
   public:
    /// Name of the parameter.
    std::string name;
    /// Type of the option.
    ApplicationCommandOption::Type type;

    DLL_EXPORT InteractionDataOption(const json& data);
    DLL_EXPORT virtual ~InteractionDataOption(){};
    DLL_EXPORT static InteractionDataOption* from_json(const json& data);

    DLL_EXPORT InteractionDataOption* copy();
};

class InteractionDataStringOption : public InteractionDataOption {
   public:
    /// Value of the option.
    std::string value;

    DLL_EXPORT InteractionDataStringOption(const json& data);
};

class InteractionDataIntegerOption : public InteractionDataOption {
   public:
    /// Value of the option.
    int value;

    DLL_EXPORT InteractionDataIntegerOption(const json& data);
};

class InteractionDataNumberOption : public InteractionDataOption {
   public:
    /// Value of the option.
    double value;

    DLL_EXPORT InteractionDataNumberOption(const json& data);
};

class InteractionDataSubcommandOption : public InteractionDataOption {
   public:
    /// array of InteractionDataOption
    std::vector<InteractionDataOption*> options;

    DLL_EXPORT InteractionDataSubcommandOption(const json& data);
    DLL_EXPORT InteractionDataSubcommandOption(const InteractionDataSubcommandOption& other);
    DLL_EXPORT ~InteractionDataSubcommandOption();
};

}  // namespace DiscordCPP