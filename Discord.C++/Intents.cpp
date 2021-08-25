#include "Intents.h"

Intents Intents::Default() {
    Intents intents = Intents();
    intents.add(GUILDS);
    intents.add(BANS);
    intents.add(VOICE_STATES);
    intents.add(MESSAGES);
    intents.add(TYPING);
    intents.add(DIRECT_MESSAGES);
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
