#include "Intents.h"

Intents Intents::Default() {
    Intents intents = Intents();
    intents.add(GUILDS);
    intents.add(VOICE_STATES);
    return intents;
}

Intents Intents::All() {
    Intents intents = Intents::Default();
    intents.add(BANS);
    intents.add(EMOJIS);
    intents.add(MESSAGES);
    intents.add(REACTIONS);
    intents.add(TYPING);
    intents.add(DIRECT_MESSAGES);
    intents.add(DIRECT_REACTIONS);
    intents.add(DIRECT_TYPING);
    return intents;
}

void Intents::add(unsigned int intent) {
    this->intents |= intent;
}

void Intents::remove(unsigned int intent) {
    this->intents ^= intent;
}

unsigned int Intents::getIntents() {
    return this->intents;
}
