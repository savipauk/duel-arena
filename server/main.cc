#include <SDL_net.h>

#include "common.h"

#define MAX_CLIENTS 2
#define CONNECTION_AWAIT 250

bool running = false, client_connected[MAX_CLIENTS];
TCPsocket server, client[MAX_CLIENTS];
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
  darena::log << "Listening from: "
              << std::to_string(client_ip >> 24) << "."
              << std::to_string((client_ip >> 16) & 0xff) << "."
              << std::to_string((client_ip >> 8) & 0xff) << "."
              << std::to_string(client_ip & 0xff) << ":" << "\n";
  server = SDLNet_TCP_Open(&ip);
  if (!server) {
    darena::log << "SDLNet_TCP_Open Error: " << SDLNet_GetError() << "\n";
    return false;
  }

  return true;
}

void wait_for_connection(int id) {
  while (!client_connected[id]) {
    darena::log << "Waiting for message...\n";
    client[id] = SDLNet_TCP_Accept(server);
    // No connection received
    if (!client[id]) {
      // Wait for CONNECTION_AWAIT ms before checking connection again
      darena::log << "Waiting for message...\n";
      SDL_Delay(CONNECTION_AWAIT);
      continue;
    }

    // Log the client IP and port
    IPaddress* client_ip_address = SDLNet_TCP_GetPeerAddress(client[id]);
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
    int len = SDLNet_TCP_Recv(client[id], message, DARENA_MAX_MESSAGE_LENGTH);
    if (!len) {
      darena::log << "SDLNet_Tcp_Recv Error: " << SDLNet_GetError() << "\n";
      break;
    }

    darena::log << "Received: \n" << std::string(message, len) << "\n";
    break;
  }

  SDLNet_TCP_Close(client[id]);
}

int main() {
  darena::log << "Starting server...\n";

  for (int i = 0; i < MAX_CLIENTS; i++) {
    client_connected[i] = false;
  }

  running = initialize();

  if (!running) {
    return 1;
  }

  darena::log << "Started server.\n";
  darena::log << "Waiting for clients to try to connect.\n";

  client_id = 0;
  wait_for_connection(client_id);
  read_message(client_id);

  // client_id = 1;
  // wait_for_connection(client_id);

  SDLNet_Quit();
  darena::log << "Server ended.\n";

  return 0;
}
