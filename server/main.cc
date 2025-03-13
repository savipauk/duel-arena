#include <SDL_net.h>

#include "common.h"
#include "game_master.h"
#include "server_lib.h"

bool game_running = false;
TCPsocket server_listening_socket, client_communication_socket[MAX_CLIENTS];
int client_id = 0;

int main() {
  darena::GameMaster game_master{};

  darena::log << "Starting server...\n";

  darena::TCPServer server{};
  game_running = server.initialize();

  if (!game_running) {
    return 1;
  }

  darena::log << "Started server.\n";
  bool noerr = true;

  darena::log << "Generating heightmaps\n";
  // Generate heightmaps
  std::vector<darena::IslandPoint> left_island_heightmap =
      game_master.generate_heightmap(ISLAND_NUM_OF_POINTS);
  std::vector<darena::IslandPoint> right_island_heightmap =
      game_master.generate_heightmap(ISLAND_NUM_OF_POINTS);

  // Stores the buffers containing heightmap information which are sent to the
  // clients
  std::vector<msgpack::sbuffer> buffers;

  darena::log << "Waiting for clients to try to connect.\n";
  for (client_id = 0; client_id < MAX_CLIENTS; client_id++) {
    noerr = server.wait_for_connection(client_id);
    if (!noerr) {
      return 1;
    }

    noerr = server.read_message(client_id);
    if (!noerr) {
      return 1;
    }

    buffers.emplace_back();

    // Pack the heightmaps information in the buffer
    msgpack::packer<msgpack::sbuffer> packer(&buffers[client_id]);
    packer.pack_array(2);
    packer.pack(left_island_heightmap);
    packer.pack(right_island_heightmap);
  }

  for (int i = 0; i < client_id; i++) {
    noerr = server.send_response(i, std::move(buffers[i]));
    if (!noerr) {
      return 1;
    }
  }

  while(1) {

  }

  server.cleanup();
  darena::log << "Server ended.\n";

  return 0;
}
