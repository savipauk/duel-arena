#pragma once

#include <memory>
#include <string>

#include "client_lib.h"
#include "game_state.h"
#include "island.h"
#include "player.h"

namespace darena {

class GameState;

struct Game {
  std::string username;
  std::string server_ip;
  darena::TCPClient client;
  std::unique_ptr<darena::GameState> state;
  std::unique_ptr<darena::Player> player;
  std::unique_ptr<darena::Island> left_island;
  std::unique_ptr<darena::Island> right_island;

  Game() : client(server_ip, username) {
    state = std::make_unique<GSConnected>();
  }
  // Sets the new state
  void set_state(std::unique_ptr<GameState> new_state);

  // Connects to the server
  bool connect_to_server();

  // Waits for the server to send island data
  bool get_island_data();

  // Update functions
  void process_input(SDL_Event* e);
  void update(float delta_time);
  void render();

  // Placeholder function to test functionality
  std::vector<darena::IslandPoint> generate(const Position& position,
                                            int num_of_points);
};

}  // namespace darena
