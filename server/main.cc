#include <SDL_net.h>

#include "server_lib.h"
#include "common.h"

bool game_running = false;
TCPsocket server_listening_socket, client_communication_socket[MAX_CLIENTS];
int client_id = 0;

int main() {
  darena::log << "Starting server...\n";

  darena::TCPServer server{};
  game_running = server.initialize();

  if (!game_running) {
    return 1;
  }

  darena::log << "Started server.\n";
  darena::log << "Waiting for clients to try to connect.\n";

  client_id = 0;
  bool noerr = server.wait_for_connection(client_id);
  if (!noerr) {
    return 1;
  }

  noerr = server.read_message(client_id);
  if (!noerr) {
    return 1;
  }
  
  noerr = server.send_response(client_id);
  if (!noerr) {
    return 1;
  }

  // client_id = 1;
  // wait_for_connection(client_id);

  server.cleanup();
  darena::log << "Server ended.\n";

  return 0;
}
