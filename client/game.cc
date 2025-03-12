#include "game.h"

#include "client_lib.h"

namespace darena {

bool Game::connect_to_server() {
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

  std::optional<msgpack::object> response = client.get_connection_response();
  if (!response.has_value()) {
    return false;
  }

  // darena::log << "obj Received: " << obj << "\n";
  darena::TCPMessage message;
  response->convert(message);

  darena::log << "id: " << message.id << "\tmsg: " << message.data << "\n";

  // darena::log << "char[1024] Received: " << msg << "\n";

  return true;
}

}  // namespace darena
