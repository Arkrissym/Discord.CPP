cmake_minimum_required( VERSION 3.5 )

project( discord_cpp )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

include_directories( ${CMAKE_STAGING_PREFIX}/usr/include )
link_directories( ${CMAKE_STAGING_PREFIX}/usr/lib )

FILE(GLOB PROJECT_SOURCES Discord.C++/*.cpp)
FILE(GLOB PROJECT_HEADERS Discord.C++/*.h)

add_library( ${PROJECT_NAME} SHARED ${PROJECT_SOURCES} )

target_include_directories( ${PROJECT_NAME} PRIVATE Discord.C++ )

if(MSVC)
      set( CMAKE_CXX_FLAGS " /MP /O2 /GL /GS /Wall /sdl /Gd /EHa " )
else(MSVC)
      set( CMAKE_CXX_FLAGS " -g -Wall -Wextra -Ofast " )
endif(MSVC)

if(VCPKG_TOOLCHAIN)
      find_package(unofficial-sodium CONFIG REQUIRED)
      find_package(Opus CONFIG REQUIRED)
      find_package(Boost COMPONENTS system filesystem REQUIRED)
      find_package(cpprestsdk CONFIG REQUIRED)

      target_link_libraries(${PROJECT_NAME}
                        unofficial-sodium::sodium
                        Opus::opus
                        Boost::system
                        Boost::filesystem
                        cpprestsdk::cpprest
                        cpprestsdk::cpprestsdk_zlib_internal
                        cpprestsdk::cpprestsdk_boost_internal
                        cpprestsdk::cpprestsdk_brotli_internal)
else(VCPKG_TOOLCHAIN)
      target_link_libraries(${PROJECT_NAME} sodium opus cpprest)
endif(VCPKG_TOOLCHAIN)

install(TARGETS discord_cpp DESTINATION lib)
install(FILES ${PROJECT_HEADERS} DESTINATION include/discord_cpp)

add_executable(test_bot test_bot/main.cpp)

target_include_directories(test_bot PRIVATE Discord.C++)

if(WIN32)
      target_link_libraries(test_bot discord_cpp)
else(WIN32)
      target_link_libraries(test_bot discord_cpp pthread ssl crypto)
endif(WIN32)

install(TARGETS test_bot DESTINATION bin)
