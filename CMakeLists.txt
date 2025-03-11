cmake_minimum_required(VERSION 3.22.1)

project(DuelArena)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Generates compile_commands.json in out/ 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_net REQUIRED)

# ImGui
add_library(ImGui STATIC
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_demo.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_sdl2.cpp
    third_party/imgui/backends/imgui_impl_sdlrenderer2.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp
    third_party/imgui/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(ImGui PUBLIC
    third_party/imgui
    third_party/imgui/backends
    third_party/imgui/misc/cpp
)
target_link_libraries(ImGui PUBLIC SDL2::SDL2)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(ImGui PUBLIC SDL2::SDL2 OpenGL::GL)

# Internal libraries
# Shared by client & server
add_library(CommonLib STATIC common/common.cc) 
# This defines the SERVER prefix in the logs
target_compile_definitions(CommonLib PRIVATE COMMON)
target_include_directories(CommonLib PUBLIC common)
target_link_libraries(CommonLib PUBLIC SDL2::SDL2 SDL2_net::SDL2_net)

# Client libraries
add_library(ClientLib STATIC 
  client/client_lib.cc
  client/engine.cc
  client/game.cc
) 
# This defines the CLIENT prefix in the logs
target_compile_definitions(ClientLib PRIVATE CLIENT)
target_include_directories(ClientLib PUBLIC client)
target_link_libraries(ClientLib CommonLib SDL2::SDL2 SDL2_net::SDL2_net ImGui)

# Server libraries
add_library(ServerLib STATIC server/server_lib.cc) 
# This defines the SERVER prefix in the logs
target_compile_definitions(ServerLib PRIVATE SERVER)
target_include_directories(ServerLib PUBLIC server)
target_link_libraries(ServerLib CommonLib SDL2::SDL2 SDL2_net::SDL2_net) 

# Client executable
add_executable(DuelArenaClient client/main.cc)
# This defines the CLIENT prefix in the logs
target_compile_definitions(DuelArenaClient PRIVATE CLIENT)
target_link_libraries(DuelArenaClient ClientLib CommonLib SDL2::SDL2 SDL2_net::SDL2_net ImGui) 

# Server executable
add_executable(DuelArenaServer server/main.cc)
# This defines the SERVER prefix in the logs
target_compile_definitions(DuelArenaServer PRIVATE SERVER)
target_link_libraries(DuelArenaServer ServerLib CommonLib SDL2::SDL2 SDL2_net::SDL2_net)


