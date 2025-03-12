#pragma once

#include <SDL_net.h>

#include <memory>
#include <vector>

#include "common.h"

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540
#define FONT_SIZE 16

#define ISLAND_X_OFFSET 80
#define ISLAND_Y_OFFSET 320
#define ISLAND_HEIGHT 100
#define ISLAND_WIDTH 320
#define ISLAND_POINT_EVERY 8
#define ISLAND_NUM_OF_POINTS (ISLAND_WIDTH / ISLAND_POINT_EVERY)

#define TARGET_FPS 60

namespace darena {

// Defines the island position and height map.
struct Island {
  std::unique_ptr<darena::Position> position;
  std::vector<darena::IslandPoint> heightmap;

  Island() {}
  Island(std::unique_ptr<darena::Position> position,
         std::vector<darena::IslandPoint> heightmap)
      : position(std::move(position)), heightmap(heightmap) {}

  std::string to_string() const;
};

// TODO: This should maybe be a class with the network stuff being private
struct TCPClient {
  std::string server_ip_string;
  // TODO: This should contain the player name or something similar
  // TODO: Add serialization
  std::string username;
  IPaddress server_ip;
  TCPsocket client_communication_socket;
  SDLNet_SocketSet socket_set;

  TCPClient(const std::string& server_ip_string, const std::string& username)
      : server_ip_string(server_ip_string),
        username(username),
        server_ip(0),
        client_communication_socket(nullptr),
        socket_set(nullptr) {}

  bool initialize();
  bool send_connection_request();
  bool get_connection_response();
  void cleanup();

  std::vector<darena::IslandPoint> convert_data_to_island_point();
};

std::vector<darena::IslandPoint> create_heightmap(int num_of_points);

}  // namespace darena
