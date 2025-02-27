# Discord.CPP [![Build Status](https://github.com/arkrissym/discord.cpp/actions/workflows/build.yml/badge.svg)](https://github.com/Arkrissym/Discord.CPP/actions/workflows/build.yml) [![Code Quality](https://github.com/arkrissym/discord.cpp/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/Arkrissym/Discord.CPP/actions/workflows/codeql-analysis.yml)
Discord.CPP or Discord.C++ is a C++ library for interaction with the Discord API.

## Documentation
Doxygen-generated documentation is available [here](https://arkrissym.github.io/Discord.CPP).

## Dependencies
- [boost-beast](https://github.com/boostorg/beast)
- [certify](https://github.com/djarek/certify)
- [nlohman-json](https://github.com/nlohmann/json)
- boost-process
- libsodium
- libopus

The communication with Discord is build around boost-beast(using certify to handle certificates) and nlohman-json. \
Libsodium and libopus are needed to encrypt voice data.

## Features
The library is still in an early state of development, so many features are missing but will be added somewhere in the future.
However below is an overview of features based on Discord's [library comparision table](https://discordapi.com/unofficial/comparison.html).

### REST API
#### Channel management
|Feature   |working? |
|----------|---------|
|Create    |No       |
|Delete    |Yes      |
|DM        |Yes      |
|History   |Yes      |
|Info      |Read-only|
|Permission|Read-only|

#### Invites
|Feature   |working? |
|----------|---------|
|Create|No|
|Delete|No|
|Info|No|
|Join|No|

#### Message management
|Feature   |working? |
|----------|---------|
|Send|Yes|
|Send File|No|
|Edit|Yes|
|Delete|Yes|

#### Role management
|Feature   |working? |
|----------|---------|
|Create|No|
|Delete|No|
|Edit|No|
|Info|Yes|

#### Server management
|Feature   |working? |
|----------|---------|
|Create|No|
|Delete|Yes|
|Edit|No|
|Info|Yes|
|Kick|Yes|
|Ban|Yes|
|Unban|Yes|
|Ban List|No|
|Change owner|No|

#### User management
|Feature   |working? |
|----------|---------|
|Manage Nicknames|No|
|Move Users in Voicechannels|No|
|Mute/Defean Users|No|

#### Other
|Feature   |working? |
|----------|---------|
|Edit Profile|  No|
|Send typing|   No|

### Gateway
#### Sharding
|Feature   |working? |
|----------|---------|
|Manual sharding|Yes|
|Auto sharding|Yes|

#### Channel Events
|Event     |handled? |
|----------|---------|
|Create|Yes|
|Delete|Yes|
|Update|Yes|

#### Message Events
|Event     |handled? |
|----------|---------|
|Receive|Yes|
|Delete|Yes|
|Update|Yes|

#### Role Events
|Event     |handled? |
|----------|---------|
|Create|No|
|Delete|No|
|Update|No|

#### Presence
|Feature   |working? |
|----------|---------|
|Receive|No|
|Send|Yes|

#### Server Events
|Event     |handled? |
|----------|---------|
|Create|Yes|
|Delete|Yes|
|Update|Yes|
|Ban|Yes|
|Unban|Yes|

#### User Events
|Event     |handled? |
|----------|---------|
|Join|Yes|
|Leave|Yes|
|Typing|Yes|

#### Voice
|Feature   |working? |
|----------|---------|
|Receive|No|
|Send|Yes|
|Multi-Server|Yes|
|State Update|Yes|
