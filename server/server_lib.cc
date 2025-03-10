#include "server_lib.h"

#include "common.h"

namespace darena {

bool TCPServer::initialize() {
  if (SDLNet_Init() == -1) {
    darena::log << "SDLNet_Init Error: " << SDLNet_GetError() << "\n";
    return false;
  }

  IPaddress server_ip;
  if (SDLNet_ResolveHost(&server_ip, NULL, DARENA_PORT) == -1) {
    darena::log << "SDLNet_ResolveHost Error: " << SDLNet_GetError() << "\n";
    return false;
  }

  uint32_t client_ip = server_ip.host;
  darena::log << "Listening from: " << unit32_t_address_to_string(client_ip)
              << "\n";

  server_listening_socket = SDLNet_TCP_Open(&server_ip);

  if (!server_listening_socket) {
    darena::log << "SDLNet_TCP_Open Error: " << SDLNet_GetError() << "\n";
    return false;
  }


  socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS);

  return true;
}

bool TCPServer::wait_for_connection(int id) {
  if (client_connected[id]) {
    darena::log << "Client with id " << std::to_string(id)
                << " already connected!\n";
    return false;
  }

  while (!client_connected[id]) {
    darena::log << "Waiting for connection...\n";
    client_communication_socket[id] =
        SDLNet_TCP_Accept(server_listening_socket);

    // No connection received
    if (!client_communication_socket[id]) {
      // Wait for DARENA_CONNECTION_AWAIT ms before checking connection again
      SDL_Delay(DARENA_CONNECTION_AWAIT);
      continue;
    }

    // Log the client IP and port
    IPaddress* client_ip_address =
        SDLNet_TCP_GetPeerAddress(client_communication_socket[id]);

    if (!client_ip_address) {
      darena::log << "SDLNet_TCP_GetPeerAddress Error: " << SDLNet_GetError()
                  << "\n";
      return false;
    }

    darena::log << "Accepted a connection from "
                << darena::ipaddress_to_string(client_ip_address) << "\n";

    client_connected[id] = true;
  }

  return true;
}
bool TCPServer::read_message(int id) {
  SDLNet_TCP_AddSocket(socket_set, client_communication_socket[id]);

  while (true) {
    darena::log << "Waiting for message...\n";
    if (!SDLNet_CheckSockets(socket_set, DARENA_CONNECTION_AWAIT)) {
      // Wait for DARENA_CONNECTION_AWAIT ms before checking connection again
      continue;
    }

    break;
  }

  char message[DARENA_MAX_MESSAGE_LENGTH];
  int len = SDLNet_TCP_Recv(client_communication_socket[id], message,
                            DARENA_MAX_MESSAGE_LENGTH);
  if (!len) {
    darena::log << "SDLNet_Tcp_Recv Error: " << SDLNet_GetError() << "\n";
    return false;
  }

  darena::log << "Received: \n" << std::string(message, len) << "\n";
  return true;
}

bool TCPServer::send_response(int id) {
  const char* message = "i will generate the islands for you now";
  int len = strlen(message);
  int result = SDLNet_TCP_Send(client_communication_socket[id], message, len);
  if (result < len) {
    darena::log << "SDLNet_TCP_Send Error: " << SDLNet_GetError() << "\n";
  }
  darena::log << "Sent response to client " << std::to_string(id) << ".\n";

  return true;
}

void TCPServer::cleanup() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client_connected[i]) {
      SDLNet_TCP_DelSocket(socket_set, client_communication_socket[i]);
    }
  }
  if (socket_set) {
    SDLNet_FreeSocketSet(socket_set);
    socket_set = nullptr;
  }
  SDLNet_TCP_Close(client_communication_socket[client_id]);
  SDLNet_Quit();
}

}  // namespace darena
