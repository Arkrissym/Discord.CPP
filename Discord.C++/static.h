#pragma once
#define VERSION		"0.1.5"

#define GATEWAY_URL	"wss://gateway.discord.gg?v=6&encoding=json"
#define API_URL		"https://discordapp.com/api"

#define is_valid_field(name)	(data.has_field(U(name))) && (!data.at(U(name)).is_null())