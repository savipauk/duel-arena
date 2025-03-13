#pragma once

#include <SDL.h>

#include <memory>
#include <thread>

#include "game.h"

namespace darena {

struct Engine {
  SDL_Window* window;
  SDL_GLContext gl_context;
  std::unique_ptr<Game> game;
  bool game_running;
  uint64_t last_frame_time;
  float fps;
  std::thread network_thread;

  Engine()
      : window(nullptr),
        gl_context(nullptr),
        game(std::make_unique<Game>()),
        game_running(false),
        last_frame_time(SDL_GetTicks64()),
        fps(0) {}

  // Initializes the SDL window and renderer
  bool initialize();

  // Setup the imgui context
  // void setup_imgui_context();

  // Does game preprocessing (TCP server connection) and then runs the game loop
  // of process_input(), update() and render()
  bool run();

  // Processes the user input
  void process_input();

  // Update function for game logic
  void update();

  // Render function with draw calls
  bool render();

  // Cleanup function that destroys the window and renderer
  void cleanup();

  // Connect to server
  void job_connect_to_server();

  // Get island data 
  void job_get_island_data();
};

}  // namespace darena
