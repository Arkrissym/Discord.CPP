#pragma once

#include <string>
#include <vector>

#include "Emoji.h"

namespace DiscordCPP {
class MessageReaction {
   public:
    class CountDetails {
       private:
        unsigned int normal;
        unsigned int burst;

       public:
        DLL_EXPORT CountDetails(const json& data);
        DLL_EXPORT unsigned int get_normal() { return normal; }
        DLL_EXPORT unsigned int get_burst() { return burst; }
    };

   private:
    /// Total number of times this emoji has been used to react (including super reacts)
    unsigned int count;
    /// reaction bount details
    CountDetails count_details;
    /// Whether the current user reacted using this emoji
    bool me;
    /// Whether the current user super-reacted using this emoji
    bool me_burst;
    /// emoji information
    Emoji emoji;
    /// hex colors used for super reraction
    std::vector<std::string> burst_colors;

   public:
    DLL_EXPORT MessageReaction(const json& data, const std::string& token);

    /// @return Total number of times this emoji has been used to react (including super reacts)
    DLL_EXPORT unsigned int get_count() { return count; }
    /// reaction bount details
    DLL_EXPORT CountDetails get_count_details() { return count_details; }
    /// @return Whether the current user reacted using this emoji
    DLL_EXPORT bool is_mine() { return me; }
    /// @return Whether the current user super-reacted using this emoji
    DLL_EXPORT bool is_my_burst() { return me_burst; }
    /// @return emoji information
    DLL_EXPORT Emoji get_emoji() { return emoji; }
    /// @return hex colors used for super reraction
    DLL_EXPORT std::vector<std::string> get_burst_colors() { return burst_colors; }
};
}  // namespace DiscordCPP
