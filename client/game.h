#pragma once

#include <memory>
#include <string>

#include "client_lib.h"

namespace darena {

enum GameState {
  INITIAL,
  CONNECTING,
  WAIT_FOR_ISLAND_DATA,
  CONNECTED,
  DISCONNECTED
};

struct Game {
  std::string username;
  std::string server_ip;
  std::unique_ptr<darena::Island> left_island;
  std::unique_ptr<darena::Island> right_island;
  darena::TCPClient client;
  GameState connection;

  Game()
      : username("Player"),
        server_ip("127.0.0.1"),
        connection(INITIAL),
        client(darena::TCPClient{server_ip, username}) {
    left_island = std::make_unique<darena::Island>();
    right_island = std::make_unique<darena::Island>();

    darena::Position left_island_position{ISLAND_X_OFFSET, ISLAND_Y_OFFSET};
    left_island->position =
        std::make_unique<darena::Position>(left_island_position);

    darena::Position right_island_position{
        WINDOW_WIDTH - ISLAND_X_OFFSET - ISLAND_WIDTH, ISLAND_Y_OFFSET};
    right_island->position =
        std::make_unique<darena::Position>(right_island_position);
  }

  // Connects to the server
  bool connect_to_server();

  // Waits for the server to send island data
  bool get_island_data();

  // OpenGL draw calls for the island
  void draw_islands();
};

}  // namespace darena
