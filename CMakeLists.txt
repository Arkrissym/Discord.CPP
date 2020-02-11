cmake_minimum_required( VERSION 3.5 )

project( discord_cpp )

set( CMAKE_CXX_FLAGS " -g -Wall -Wextra -O2 -Wl,--no-as-needed -fPIC -std=c++17 " )

include_directories( ${CMAKE_STAGING_PREFIX}/usr/include )
link_directories( ${CMAKE_STAGING_PREFIX}/usr/lib )

set ( src_discord_cpp
      Discord.C++/Activity.cpp
      Discord.C++/Channel.cpp
      Discord.C++/Discord.cpp
      Discord.C++/DiscordObject.cpp
      Discord.C++/DMChannel.cpp
      Discord.C++/Embed.cpp
      Discord.C++/Exceptions.cpp
      Discord.C++/FileAudioSource.cpp
      Discord.C++/Gateway.cpp
      Discord.C++/Guild.cpp
      Discord.C++/GuildChannel.cpp
      Discord.C++/Logger.cpp
      Discord.C++/MainGateway.cpp
      Discord.C++/Member.cpp
      Discord.C++/Message.cpp
      Discord.C++/static.cpp
      Discord.C++/TextChannel.cpp
      Discord.C++/User.cpp
      Discord.C++/VoiceChannel.cpp
      Discord.C++/VoiceClient.cpp
      Discord.C++/VoiceGateway.cpp
      Discord.C++/VoiceState.cpp
)

add_library( ${PROJECT_NAME} SHARED ${src_discord_cpp} )

target_include_directories( ${PROJECT_NAME} PRIVATE Discord.C++ )

target_link_libraries( ${PROJECT_NAME} sodium )
target_link_libraries( ${PROJECT_NAME} cpprest )
target_link_libraries( ${PROJECT_NAME} opus )
target_link_libraries( ${PROJECT_NAME} pthread )
target_link_libraries( ${PROJECT_NAME} crypto )
target_link_libraries( ${PROJECT_NAME} ssl )
