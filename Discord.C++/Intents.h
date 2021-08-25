#pragma once
#include "static.h"

class Intents {
   private:
    unsigned int intents = 0;

   public:
    /**	Creates an Intents object with all non-privileged intents the library can handle.
	*	@return Intents intents used when connecting to the gateway.
	*/
    DLL_EXPORT static Intents Default();

    /// Enable an intent.
    DLL_EXPORT void add(unsigned int intent);
    /// Disable an intent.
    DLL_EXPORT void remove(unsigned int intent);
    /// Get Intents as integer.
    DLL_EXPORT unsigned int getIntents();

    /**	Intent for Guild relevant events.
	*	This affects the internal guild cache
	*/
    static const unsigned int GUILDS = 1 << 0;

    /**	Intent for Guild Member relevant events. This is a privileged intent!
	*	This affects the following events:
	*		- on_user_join
	*		- on_user_remove
	*/
    static const unsigned int MEMBERS = 1 << 1;

    /** Intent for Guild Member relevant events.
	*	This affects the following events:
	*		- on_user_ban
	*		- on_user_unban
	*/
    static const unsigned int BANS = 1 << 2;

    /**	Intent for Guild Emoji relevant events.
	*	This affects nothing as emojis are not yet implemented.
	*/
    static const unsigned int EMOJIS = 1 << 3;

    /**	Intent for Guild Integration relevant events.
	*	This affects nothing as integrations are not yet implemented.
	*/
    static const unsigned int INTEGRATIONS = 1 << 4;

    /**	Intent for Guild Webhook relevant events.
	*	This affects nothing as webhooks are not yet implemented.
	*/
    static const unsigned int WEBHOOKS = 1 << 5;

    /**	Intent for Guild Invite relevant events.
	*	This affects nothing as invites are not yet implemented.
	*/
    static const unsigned int INVITES = 1 << 6;

    /**	Intent for Guild relevant events.
	*	This affects the internal voice handling.
	*/
    static const unsigned int VOICE_STATES = 1 << 7;

    /**	Intent for Guild Presence relevant events. This is a privileged intent!
	*	This affects nothing as presences are not yet implemented.
	*/
    static const unsigned int PRESENCES = 1 << 8;

    /**	Intent for Guild Message events.
	*	This affects the following events:
	*		- on_message
	*/
    static const unsigned int MESSAGES = 1 << 9;

    /**	Intent for Guild Reaction relevant events.
	*	This affects nothing as reactions are not yet implemented.
	*/
    static const unsigned int REACTIONS = 1 << 10;

    /**	Intent for Guild Typing relevant events.
	*	This affects the following events:
	*		- on_typing_start
	*/
    static const unsigned int TYPING = 1 << 11;

    /**	Intent for Direct Message relevant events.
	*	This affects the following events:
	*		- on_message
	*/
    static const unsigned int DIRECT_MESSAGES = 1 << 12;

    /**	Intent for Direct Message Reaction relevant events.
	*	This affects nothing as reactions are not yet implemented.
	*/
    static const unsigned int DIRECT_REACTIONS = 1 << 13;

    /**	Intent for Direct Message Typing relevant events.
	*	This affects the following events:
	*		- on_typing_start
	*/
    static const unsigned int DIRECT_TYPING = 1 << 14;
};
