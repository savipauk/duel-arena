#include "game.h"
#include "client_lib.h"

namespace darena {

bool Game::connect_to_server() {
  // const char* server_ip = "127.0.0.1";
  // const char* message = "Hello Server";
  bool noerr;

  darena::TCPClient client{server_ip, username};

  noerr = client.initialize();
  if (!noerr) {
    return false;
  }

  noerr = client.send_connection_request();
  if (!noerr) {
    return false;
  }

  noerr = client.get_connection_response();
  if (!noerr) {
    return false;
  }

  return true;
}

}  // namespace darena
