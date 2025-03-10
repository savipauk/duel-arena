#pragma once

#include <SDL.h>

namespace darena {

struct Engine {
  SDL_Window* window;
  SDL_Renderer* renderer;
  bool game_running;
  uint64_t last_frame_time;
  float fps;

  Engine()
      : window(NULL),
        renderer(NULL),
        game_running(false),
        last_frame_time(0),
        fps(0) {}

  // Initializes the SDL window and renderer
  bool initialize();

  // Does game preprocessing (TCP server connection) and then runs the game loop
  // of process_input(), update() and render()
  bool run();

  // Processes the user input
  void process_input();

  // Update function for game logic
  void update();

  // Render function with draw calls
  void render();

  // Cleanup function that destroys the window and renderer
  void cleanup();
};

}  // namespace darena
