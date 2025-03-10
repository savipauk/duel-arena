#include "client_lib.h"

#include <SDL_net.h>

#include <random>

namespace darena {

std::string Island::to_string() const {
  std::string output = "Island(" + position->to_string();
  for (auto it = heightmap.begin(); it != heightmap.end(); it++) {
    output.append(it->to_string());
  }
  output.append(")");
  return output;
}

bool TCPClient::initialize() {
  if (SDLNet_ResolveHost(&server_ip, server_ip_string, DARENA_PORT) == -1) {
    darena::log << "SDLNet_ResolveHost Error: " << SDLNet_GetError() << "\n";
    return false;
  }

  client_communication_socket = SDLNet_TCP_Open(&server_ip);
  if (!client_communication_socket) {
    darena::log << "SDLNet_TCP_Open Error: " << SDLNet_GetError() << "\n";
    return false;
  }

  return true;
}

bool TCPClient::send_connection_request() {
  int len = strlen(message);
  int result = SDLNet_TCP_Send(client_communication_socket, message, len);
  if (result < len) {
    darena::log << "SDLNet_TCP_Send Error: " << SDLNet_GetError() << "\n";
    return false;
  }
  darena::log << "Sent message to server.\n";
  return true;
}

bool TCPClient::get_connection_response() {
  bool socket_ready = false;
  socket_set = SDLNet_AllocSocketSet(1);
  SDLNet_TCP_AddSocket(socket_set, client_communication_socket);

  while (!socket_ready) {
    darena::log << "Waiting for message...\n";
    if (!SDLNet_CheckSockets(socket_set, DARENA_CONNECTION_AWAIT)) {
      // Wait for DARENA_CONNECTION_AWAIT ms before checking connection again
      continue;
    }

    // Log the server IP and port
    IPaddress* server_ip_address =
        SDLNet_TCP_GetPeerAddress(client_communication_socket);
    if (!server_ip_address) {
      darena::log << "SDLNet_TCP_GetPeerAddress Error: " << SDLNet_GetError()
                  << "\n";
      return false;
    }

    darena::log << "Incoming message from "
                << ipaddress_to_string(server_ip_address) << "\n";

    socket_ready = true;
  }

  char message[1024];
  int len = SDLNet_TCP_Recv(client_communication_socket, message, 1024);
  if (!len) {
    darena::log << "SDLNet_TCP_Recv Error: " << SDLNet_GetError() << "\n";
    return false;
  }
  darena::log << "Received: \n" << std::string(message, len) << "\n";

  return true;
}

void TCPClient::cleanup() {
  SDLNet_TCP_DelSocket(socket_set, client_communication_socket);
  SDLNet_FreeSocketSet(socket_set);
  SDLNet_TCP_Close(client_communication_socket);
  SDLNet_Quit();
}

// Used to randomly generate numbers in create_heightmap()
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

// TODO: Move this function to the server. The server should generate the height
// maps and send them to the clients
std::vector<IslandPoint> create_heightmap(int num_of_points) {
  std::vector<IslandPoint> output = {};

  int last_height = 50 + (dis(gen) - 0.5) * 50;
  for (int i = 0; i < num_of_points; i++) {
    last_height += (dis(gen) - 0.5) * 10;

    if (last_height < 25) {
      last_height = 25;
    } else if (last_height > 75) {
      last_height = 75;
    }

    output.emplace_back(last_height);
  }

  return output;
}

}  // namespace darena
