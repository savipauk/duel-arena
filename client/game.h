#pragma once

#include <string>

namespace darena {

enum Connection { INITIAL, CONNECTING, CONNECTED, DISCONNECTED };

struct Game {
  std::string username;
  std::string server_ip;
  Connection connection;

  Game() : username("Player"), server_ip("127.0.0.1"), connection(INITIAL) {}

  // Connects to the server
  bool connect_to_server();
};

}  // namespace darena
