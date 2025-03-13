#pragma once

#include <memory>
#include <string>

#include "client_lib.h"
#include "game_state.h"

namespace darena {

class GameState;

struct Game {
  std::string username;
  std::string server_ip;
  std::unique_ptr<darena::Island> left_island;
  std::unique_ptr<darena::Island> right_island;
  darena::TCPClient client;
  std::unique_ptr<darena::GameState> state;

  Game()
      : client(server_ip, username),
        left_island(std::make_unique<darena::Island>()),
        right_island(std::make_unique<darena::Island>()) {
        state = std::make_unique<GSInitial>();
    left_island->position =
        std::make_unique<darena::Position>(ISLAND_X_OFFSET, ISLAND_Y_OFFSET);
    right_island->position = std::make_unique<darena::Position>(
        WINDOW_WIDTH - ISLAND_X_OFFSET - ISLAND_WIDTH, ISLAND_Y_OFFSET);
  }
  // Sets the new state
  void set_state(std::unique_ptr<GameState> new_state);

  // Connects to the server
  bool connect_to_server();

  // Waits for the server to send island data
  bool get_island_data();

  // OpenGL draw calls for the island
  void draw_islands();
};

}  // namespace darena
