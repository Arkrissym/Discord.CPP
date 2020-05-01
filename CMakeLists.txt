cmake_minimum_required( VERSION 3.5 )

project( discord_cpp )

set( CMAKE_CXX_FLAGS " -g -Wall -Wextra -O2 -Wl,--no-as-needed -fPIC " )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

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

install(TARGETS discord_cpp DESTINATION lib)
install(FILES 
      Discord.C++/Activity.h
      Discord.C++/AudioSource.h
      Discord.C++/Channel.h
      Discord.C++/Discord.h
      Discord.C++/DiscordObject.h
      Discord.C++/DMChannel.h
      Discord.C++/Embed.h
      Discord.C++/Exceptions.h
      Discord.C++/FileAudioSource.h
      Discord.C++/Gateway.h
      Discord.C++/Guild.h
      Discord.C++/GuildChannel.h
      Discord.C++/Logger.h
      Discord.C++/MainGateway.h
      Discord.C++/Member.h
      Discord.C++/Message.h
      Discord.C++/static.h
      Discord.C++/TextChannel.h
      Discord.C++/User.h
      Discord.C++/VoiceChannel.h
      Discord.C++/VoiceClient.h
      Discord.C++/VoiceGateway.h
      Discord.C++/VoiceState.h
      DESTINATION include/discord_cpp
  )
