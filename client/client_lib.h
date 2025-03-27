#pragma once

#include <SDL_net.h>
#include <msgpack/sbuffer.h>

#include <optional>
#include <vector>

#include "common.h"
#include "msgpack.hpp"

#define FONT_SIZE 16

#define TARGET_FPS 60

namespace darena {

// TODO: This should maybe be a class with the network stuff being private
struct TCPClient {
  std::string server_ip_string;  // TODO: This should be passed as an argument
                                 // in the functon
  std::string username;          // TODO: This too
  IPaddress server_ip;           // TODO: This too
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
  bool wait_for_message();
  std::optional<msgpack::unpacked> get_connection_response();
  bool send_turn_data(std::unique_ptr<darena::ClientTurn> turn_data);
  void cleanup();

  std::vector<darena::IslandPoint> convert_data_to_island_point();
};

std::vector<darena::IslandPoint> create_heightmap(int num_of_points);

}  // namespace darena
