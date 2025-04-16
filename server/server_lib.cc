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
    darena::log << "Waiting for turn data from id " << id << "...\n";

    if (!SDLNet_CheckSockets(socket_set, DARENA_CONNECTION_AWAIT)) {
      // Wait for DARENA_CONNECTION_AWAIT ms before checking connection again
      continue;
    }
    if (!SDLNet_SocketReady(client_communication_socket[id])) {
      continue;
    }
    break;
  }

  // Read the message length first
  uint32_t message_size;
  int len = SDLNet_TCP_Recv(client_communication_socket[id], &message_size,
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
  len = SDLNet_TCP_Recv(client_communication_socket[id], message.data(),
                        message_size);
  if (!len) {
    darena::log << "SDLNet_Tcp_Recv Error, len=" << len
                << "\nError: " << SDLNet_GetError() << "\n";
    return false;
  }
  darena::log << "Received message from id: " << id << "\n";

  msgpack::unpacked result;
  msgpack::unpack(result, message.data(), message_size);
  msgpack::object obj = result.get();

  darena::ClientConnectionRequest tcp_message;
  obj.convert(tcp_message);

  darena::log << "player_name: " << tcp_message.player_name << "\n";

  return true;
}

bool TCPServer::send_response(int id, msgpack::sbuffer data) {
  uint32_t message_size = htonl(data.size());
  int result = SDLNet_TCP_Send(client_communication_socket[id], &message_size,
                               sizeof(message_size));
  if (result < sizeof(message_size)) {
    darena::log << "SDLNet_TCP_Send Error, len=" << result
                << "\nError: " << SDLNet_GetError() << "\n";
    return false;
  }
  darena::log << "Sent message length to client " << std::to_string(id)
              << ".\n";

  result = SDLNet_TCP_Send(client_communication_socket[id], data.data(),
                           data.size());
  if (result < data.size()) {
    darena::log << "SDLNet_TCP_Send Error, len=" << result
                << "\nError: " << SDLNet_GetError() << "\n";
    return false;
  }
  darena::log << "Sent response to client " << std::to_string(id) << ".\n";

  return true;
}

bool TCPServer::get_turn_data(int id) {
  while (true) {
    darena::log << "Waiting for turn data from id " << id << "...\n";

    if (!SDLNet_CheckSockets(socket_set, DARENA_CONNECTION_AWAIT)) {
      // Wait for DARENA_CONNECTION_AWAIT ms before checking connection again
      continue;
    }
    if (!SDLNet_SocketReady(client_communication_socket[id])) {
      continue;
    }
    break;
  }

  // Read the message length first
  uint32_t message_size;
  int len = SDLNet_TCP_Recv(client_communication_socket[id], &message_size,
                            sizeof(message_size));
  if (!len) {
    darena::log << "SDLNet_TCP_Recv Error, len=" << len
                << "\nError: " << SDLNet_GetError() << "\n ";
    return {};
  }
  message_size = ntohl(message_size);
  darena::log << "next message size: " << message_size << "\n";

  // Allocate buffer for the message
  std::vector<char> message(message_size);
  len = SDLNet_TCP_Recv(client_communication_socket[id], message.data(),
                        message_size);
  if (!len) {
    darena::log << "SDLNet_Tcp_Recv Error, len=" << len
                << "\nError: " << SDLNet_GetError() << "\n";
    return false;
  }
  darena::log << "Received message from id: " << id << "\n";

  msgpack::unpacked result;
  msgpack::unpack(result, message.data(), message_size);
  msgpack::object obj = result.get();

  turn_data = std::make_unique<darena::ClientTurn>();
  obj.convert(*turn_data);

  std::string movements = "";
  std::string angles = "";
  for (int i : turn_data->movements) {
    movements.append(std::to_string(i));
    movements.append(" ");
  }
  for (int i : turn_data->angle_changes) {
    angles.append(std::to_string(i));
    angles.append(" ");
  }

  darena::log << turn_data->id << "\tMovements: " << movements
              << "\tAngles: " << angles << "\t" << turn_data->shot_angle << "\t"
              << turn_data->shot_power << "\n";

  return true;
}

void TCPServer::trim_turn_data() {
  if (!turn_data) {
    return;
  }

  std::vector<int> trimmed_movements = {};
  int previous_i = 0;
  int n_of_zero = 0;
  for (auto it = turn_data->movements.begin(); it != turn_data->movements.end();
       it++) {
    if (n_of_zero >= MAX_N_OF_ZERO_IN_MOVEMENT) {
      if (*it == 0) {
        continue;
      }
    }
    if (*it == 0) {
      n_of_zero++;
    } else {
      n_of_zero = 0;
    }
    trimmed_movements.emplace_back(*it);
  }

  std::string movements = "";
  for (int i : turn_data->movements) {
    movements.append(std::to_string(i));
    movements.append(" ");
  }
  darena::log << "Old movements: " << movements << "\n";
  movements = "";
  for (int i : trimmed_movements) {
    movements.append(std::to_string(i));
    movements.append(" ");
  }
  darena::log << "New movements: " << movements << "\n";
  turn_data->movements = trimmed_movements;
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
