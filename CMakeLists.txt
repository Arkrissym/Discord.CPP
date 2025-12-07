cmake_minimum_required( VERSION 3.16.0 )

project( discord_cpp )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
set( CMAKE_CXX_FLAGS " -fPIC -Ofast " )

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
      GIT_TAG        boost-1.89.0
      GIT_SHALLOW    TRUE
      GIT_PROGRESS   TRUE
)

FetchContent_Declare(boost_beast
      GIT_REPOSITORY https://github.com/boostorg/beast
      GIT_TAG        boost-1.89.0
      GIT_SHALLOW    TRUE
      GIT_PROGRESS   TRUE
)

FetchContent_Declare(nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_Declare(zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG v1.3.1
)


FetchContent_MakeAvailable(boost_beast nlohmann_json Boost zlib)

include_directories(SYSTEM ${CMAKE_STAGING_PREFIX}/usr/include
                        ${certify_SOURCE_DIR}/include
                        ${Boost_SOURCE_DIR}
                        ${zlib_SOURCE_DIR}
                  )
link_directories( ${CMAKE_STAGING_PREFIX}/usr/lib
                  ${Boost_BINARY_DIR}
                  ${zlib_BINARY_DIR}
                  )

FILE(GLOB PROJECT_SOURCES Discord.C++/*.cpp)
FILE(GLOB PROJECT_HEADERS Discord.C++/*.h)

add_library( ${PROJECT_NAME} SHARED ${PROJECT_SOURCES} )

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
target_include_directories( ${PROJECT_NAME} PRIVATE Discord.C++ )

find_package(OpenSSL REQUIRED)
find_library(Sodium NAMES sodium libsodium REQUIRED)

target_link_libraries(${PROJECT_NAME} ${Sodium} opus boost_beast Boost::system Boost::filesystem Boost::process Boost::asio nlohmann_json::nlohmann_json zlib OpenSSL::SSL OpenSSL::Crypto)

install(TARGETS discord_cpp DESTINATION lib)
install(FILES ${PROJECT_HEADERS} DESTINATION include/discord_cpp)

add_executable(test_bot test_bot/main.cpp)

target_compile_options(test_bot PRIVATE -Wall -Wextra)
target_include_directories(test_bot PRIVATE Discord.C++)
target_link_libraries(test_bot discord_cpp pthread)
