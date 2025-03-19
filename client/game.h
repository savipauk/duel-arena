#pragma once

#include <memory>
#include <string>

#include "client_lib.h"
#include "game_state.h"

namespace darena {

class GameState;

struct Player {
  darena::Position position;
  int width;
  int height;
  int move_speed = 100;
  float gravity = 0.9f;
  float curr_y_speed = 0;
  bool falling = false;

  Player(float x, float y, int width, int height)
      : position(x, y), width(width), height(height) {};
};

struct Game {
  std::string username;
  std::string server_ip;
  std::unique_ptr<darena::Island> left_island;
  std::unique_ptr<darena::Island> right_island;
  darena::TCPClient client;
  std::unique_ptr<darena::GameState> state;
  Player player;

  Game()
      : client(server_ip, username),
        left_island(std::make_unique<darena::Island>()),
        right_island(std::make_unique<darena::Island>()),
        player(100, 100, 25, 25) {
    state = std::make_unique<GSConnected>();
  }
  // Sets the new state
  void set_state(std::unique_ptr<GameState> new_state);

  // Connects to the server
  bool connect_to_server();

  // Waits for the server to send island data
  bool get_island_data();

  // OpenGL draw calls for the island
  void draw_islands();

  // Placeholder function to test functionality
  std::vector<darena::IslandPoint> generate(const Position& position, int num_of_points);
};

}  // namespace darena
