#include <SDL_net.h>

#define MAX_CLIENTS 2

namespace darena {

struct TCPServer {
  bool client_connected[MAX_CLIENTS];
  TCPsocket client_communication_socket[MAX_CLIENTS];
  TCPsocket server_listening_socket;
  SDLNet_SocketSet socket_set;
  int client_id = 0;

  TCPServer()
      : server_listening_socket(NULL),
        client_communication_socket(NULL),
        socket_set(NULL) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
      client_connected[i] = false;
    }
  }

  bool sdlnet_initialize();
  bool wait_for_connection(int id);
  bool read_message(int id);
  bool send_response(int id);
  void sdlnet_cleanup();
};

}  // namespace darena
