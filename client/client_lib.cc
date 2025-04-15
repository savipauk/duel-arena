#include "client_lib.h"

#include <SDL_net.h>

namespace darena {

bool TCPClient::initialize() {
  const char* server_ip_cc = server_ip_string.c_str();
  if (SDLNet_ResolveHost(&server_ip, server_ip_cc, DARENA_PORT) == -1) {
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
  darena::ClientConnectionRequest message{username};
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, message);

  uint32_t message_size = htonl(buffer.size());
  int result = SDLNet_TCP_Send(client_communication_socket, &message_size,
                               sizeof(message_size));
  if (result < sizeof(message_size)) {
    darena::log << "SDLNet_TCP_Send Error, len=" << result
                << "\nError: " << SDLNet_GetError() << "\n";
    return false;
  }

  message_size = ntohl(message_size);
  darena::log << "Sent message length (" << message_size << ") to server\n";

  result = SDLNet_TCP_Send(client_communication_socket, buffer.data(),
                           buffer.size());
  if (result < buffer.size()) {
    darena::log << "SDLNet_TCP_Send Error, len=" << result
                << "\nError: " << SDLNet_GetError() << "\n";
    return false;
  }
  darena::log << "Sent message to server.\n";
  return true;
}

bool TCPClient::wait_for_message() {
  bool socket_ready = false;
  SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(1);
  if (!socket_set) {
    darena::log << "SDLNet_AllocSocketSet Error: " << SDLNet_GetError() << "\n";
    return false;
  }

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

  SDLNet_FreeSocketSet(socket_set);
  return true;
}

std::optional<msgpack::unpacked> TCPClient::get_response() {
  // Read the message length first
  uint32_t message_size;
  int len = SDLNet_TCP_Recv(client_communication_socket, &message_size,
                            sizeof(message_size));
  if (!len) {
    darena::log << "SDLNet_TCP_Recv Error, len=" << len
                << "\nError: " << SDLNet_GetError() << "\n ";
    return {};
  }
  message_size = ntohl(message_size);
  darena::log << "Next message size: " << message_size << "\n";

  // Allocate buffer for the message
  std::vector<char> message(message_size);
  len = SDLNet_TCP_Recv(client_communication_socket, message.data(),
                        message_size);
  if (!len) {
    darena::log << "SDLNet_TCP_Recv Error, len=" << len
                << "\nError: " << SDLNet_GetError() << "\n ";
    return {};
  }
  darena::log << "Received a message from the server.\n";

  try {
    msgpack::unpacked result;
    msgpack::unpack(result, message.data(), message_size);
    return result;
  } catch (const std::exception& e) {
    darena::log << "Message unpack error: " << e.what() << "\n";
    return {};
  }
}

bool TCPClient::send_turn_data(std::unique_ptr<darena::ClientTurn> turn_data) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, *turn_data);

  uint32_t message_size = htonl(buffer.size());
  int result = SDLNet_TCP_Send(client_communication_socket, &message_size,
                               sizeof(message_size));
  if (result < sizeof(message_size)) {
    darena::log << "SDLNet_TCP_Send Error, len=" << result
                << "\nError: " << SDLNet_GetError() << "\n";
    return false;
  }

  message_size = ntohl(message_size);
  darena::log << "Sent message length (" << message_size << ") to server\n";

  result = SDLNet_TCP_Send(client_communication_socket, buffer.data(),
                           buffer.size());
  if (result < buffer.size()) {
    darena::log << "SDLNet_TCP_Send Error, len=" << result
                << "\nError: " << SDLNet_GetError() << "\n";
    return false;
  }
  darena::log << "Sent message to server.\n";
  return true;
}

void TCPClient::cleanup() {
  SDLNet_TCP_Close(client_communication_socket);
  SDLNet_Quit();
}

}  // namespace darena
