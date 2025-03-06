#include <SDL_net.h>

#include "common.h"

#define MAX_CLIENTS 2

bool game_running = false, client_connected[MAX_CLIENTS];
TCPsocket server_listening_socket, client_communication_socket[MAX_CLIENTS];
IPaddress ip;
int client_id = 0;

bool initialize() {
  if (SDLNet_Init() == -1) {
    darena::log << "SDLNet_Init Error: " << SDLNet_GetError() << "\n";
    return false;
  }

  if (SDLNet_ResolveHost(&ip, NULL, DARENA_PORT) == -1) {
    darena::log << "SDLNet_ResolveHost Error: " << SDLNet_GetError() << "\n";
    return false;
  }

  uint32_t client_ip = ip.host;
  darena::log << "Listening from: " << std::to_string(client_ip >> 24) << "."
              << std::to_string((client_ip >> 16) & 0xff) << "."
              << std::to_string((client_ip >> 8) & 0xff) << "."
              << std::to_string(client_ip & 0xff) << ":" << "\n";
  server_listening_socket = SDLNet_TCP_Open(&ip);

  if (!server_listening_socket) {
    darena::log << "SDLNet_TCP_Open Error: " << SDLNet_GetError() << "\n";
    return false;
  }

  return true;
}

void wait_for_connection(int id) {
  while (!client_connected[id]) {
    darena::log << "Waiting for message...\n";
    client_communication_socket[id] =
        SDLNet_TCP_Accept(server_listening_socket);
    // No connection received
    if (!client_communication_socket[id]) {
      // Wait for DARENA_CONNECTION_AWAIT ms before checking connection again
      darena::log << "Waiting for message...\n";
      SDL_Delay(DARENA_CONNECTION_AWAIT);
      continue;
    }

    // Log the client IP and port
    IPaddress* client_ip_address =
        SDLNet_TCP_GetPeerAddress(client_communication_socket[id]);
    if (!client_ip_address) {
      darena::log << "SDLNet_TCP_GetPeerAddress Error: " << SDLNet_GetError()
                  << "\n";
      continue;
    }

    uint32_t client_ip = SDL_SwapBE32(client_ip_address->host);
    darena::log << "Accepted a connection from "
                << std::to_string(client_ip >> 24) << "."
                << std::to_string((client_ip >> 16) & 0xff) << "."
                << std::to_string((client_ip >> 8) & 0xff) << "."
                << std::to_string(client_ip & 0xff) << ":"
                << std::to_string(client_ip_address->port) << "\n";

    client_connected[id] = true;
  }
}

void read_message(int id) {
  while (true) {
    char message[DARENA_MAX_MESSAGE_LENGTH];
    int len = SDLNet_TCP_Recv(client_communication_socket[id], message,
                              DARENA_MAX_MESSAGE_LENGTH);
    if (!len) {
      darena::log << "SDLNet_Tcp_Recv Error: " << SDLNet_GetError() << "\n";
      // break;
      SDL_Delay(250);
      continue;
    }

    darena::log << "Received: \n" << std::string(message, len) << "\n";
    break;
  }

}

void send_response(int id) {
  const char* message = "i will generate the islands for you now";
  int len = strlen(message);
  int result = SDLNet_TCP_Send(client_communication_socket[id], message, len);
  if (result < len) {
    darena::log << "SDLNet_TCP_Send Error: " << SDLNet_GetError() << "\n";
  }
  darena::log << "Sent response to client " << std::to_string(id) << ".\n";

  SDLNet_TCP_Close(client_communication_socket[client_id]);
}

int main() {
  darena::log << "Starting server...\n";

  for (int i = 0; i < MAX_CLIENTS; i++) {
    client_connected[i] = false;
  }

  game_running = initialize();

  if (!game_running) {
    return 1;
  }

  darena::log << "Started server.\n";
  darena::log << "Waiting for clients to try to connect.\n";

  client_id = 0;
  wait_for_connection(client_id);
  read_message(client_id);
  send_response(client_id);

  // client_id = 1;
  // wait_for_connection(client_id);

  SDLNet_Quit();
  darena::log << "Server ended.\n";

  return 0;
}
