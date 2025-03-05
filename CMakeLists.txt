cmake_minimum_required(VERSION 3.22.1)

project(DuelArena)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Generates compile_commands.json in out/ 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SDL2 (Only for the client)
find_package(SDL2 REQUIRED)

# Internal libraries
# Shared by client & server
add_library(CommonLib STATIC common/common.cc) 
target_include_directories(CommonLib PUBLIC common)

# Client libraries
add_library(ClientLib STATIC client/client_lib.cc) 
target_include_directories(ClientLib PUBLIC client)
target_link_libraries(ClientLib CommonLib SDL2::SDL2)

# Server libraries
add_library(ServerLib STATIC server/server_lib.cc) 
target_include_directories(ServerLib PUBLIC server)
# No SDL2 dependency for server
target_link_libraries(ServerLib CommonLib) 

# Third-party libraries (example)
# add_library(ThirdPartyLib STATIC thirdparty/some_lib.cc)
# target_include_directories(ThirdPartyLib PUBLIC thirdparty)

# Client executable
add_executable(DuelArenaClient client/main.cc)
# Define for logs
target_compile_definitions(DuelArenaClient PRIVATE CLIENT)
target_link_libraries(DuelArenaClient ClientLib CommonLib SDL2::SDL2)
# Includes ThirdPartyLib (currently none)
# target_link_libraries(DuelArenaClient ClientLib CommonLib ThirdPartyLib SDL2::SDL2)

# Server executable
add_executable(DuelArenaServer server/main.cc)
# Define for logs
target_compile_definitions(DuelArenaServer PRIVATE SERVER)
target_link_libraries(DuelArenaServer ServerLib CommonLib)
# Includes ThirdPartyLib (currently none)
# target_link_libraries(DuelArenaServer ServerLib CommonLib ThirdPartyLib)


