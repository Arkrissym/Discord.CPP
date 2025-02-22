cmake_minimum_required( VERSION 3.16.0...3.27.6 )

project( discord_cpp )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

set( JSON_ImplicitConversions OFF )

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(certify GIT_REPOSITORY https://github.com/djarek/certify)
FetchContent_GetProperties(certify)

set(BOOST_INCLUDE_LIBRARIES thread filesystem system process asio endian logic static_string)
set(BOOST_ENABLE_CMAKE ON)
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

FetchContent_Declare(Boost
      GIT_REPOSITORY https://github.com/boostorg/boost.git
      GIT_TAG        boost-1.84.0
      GIT_SHALLOW    TRUE
      GIT_PROGRESS   TRUE
)

FetchContent_Declare(boost_beast
      GIT_REPOSITORY https://github.com/boostorg/beast
      GIT_TAG        boost-1.84.0
      GIT_SHALLOW    TRUE
      GIT_PROGRESS   TRUE
)

FetchContent_Declare(nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

FetchContent_Declare(zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG v1.3.1
)

FetchContent_Declare(sodium
    GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
    GIT_TAG 1.0.20-RELEASE
)


FetchContent_MakeAvailable(boost_beast nlohmann_json Boost zlib sodium)

include_directories(    ${CMAKE_STAGING_PREFIX}/usr/include
                        ${certify_SOURCE_DIR}/include
                        ${Boost_SOURCE_DIR}
                        ${zlib_SOURCE_DIR}
                        ${sodium_SOURCE_DIR}
                  )
link_directories( ${CMAKE_STAGING_PREFIX}/usr/lib
                  ${Boost_BINARY_DIR}
                  ${zlib_BINARY_DIR}
                  ${sodium_BINARY_DIR}
                  )

FILE(GLOB PROJECT_SOURCES Discord.C++/*.cpp)
FILE(GLOB PROJECT_HEADERS Discord.C++/*.h)

add_library( ${PROJECT_NAME} SHARED ${PROJECT_SOURCES} )

target_include_directories( ${PROJECT_NAME} PRIVATE Discord.C++ )

if(MSVC)
      set( CMAKE_CXX_FLAGS " /MP /O2 /GL /GS /W3 /sdl /Gd /EHa " )
else(MSVC)
      set( CMAKE_CXX_FLAGS " -Wall -Wextra -Wno-nan-infinity-disabled -Ofast " )
endif(MSVC)

find_package(OpenSSL REQUIRED)

if(VCPKG_TOOLCHAIN)
      find_package(unofficial-sodium CONFIG REQUIRED)
      find_package(Opus CONFIG REQUIRED)

      target_link_libraries(${PROJECT_NAME}
                        unofficial-sodium::sodium
                        Opus::opus
                        nlohmann_json::nlohmann_json
                        Boost::system
                        Boost::filesystem
                        OpenSSL::SSL
                        OpenSSL::Crypto
                        crypt32
                        zlib)
else(VCPKG_TOOLCHAIN)
      target_link_libraries(${PROJECT_NAME} sodium opus boost_beast Boost::system Boost::filesystem Boost::process Boost::asio nlohmann_json::nlohmann_json zlib OpenSSL::SSL OpenSSL::Crypto)
endif(VCPKG_TOOLCHAIN)

install(TARGETS discord_cpp DESTINATION lib)
install(FILES ${PROJECT_HEADERS} DESTINATION include/discord_cpp)

add_executable(test_bot test_bot/main.cpp)

target_include_directories(test_bot PRIVATE Discord.C++)

if(WIN32)
      target_link_libraries(test_bot discord_cpp)
else(WIN32)
      target_link_libraries(test_bot discord_cpp pthread)
endif(WIN32)
