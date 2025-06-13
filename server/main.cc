#include <SDL_net.h>

#include <array>

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
  darena::Vec2 left_island_starting_position{ISLAND_X_OFFSET, ISLAND_Y_OFFSET};
  darena::Vec2 right_island_starting_position{
      WINDOW_WIDTH - ISLAND_X_OFFSET - ISLAND_WIDTH, ISLAND_Y_OFFSET};

  std::vector<darena::IslandPoint> left_island_heightmap =
      game_master.generate_heightmap(left_island_starting_position,
                                     ISLAND_NUM_OF_POINTS);
  std::vector<darena::IslandPoint> right_island_heightmap =
      game_master.generate_heightmap(right_island_starting_position,
                                     ISLAND_NUM_OF_POINTS);

  std::array<std::vector<darena::IslandPoint>, 2> heightmaps = {
      left_island_heightmap, right_island_heightmap};

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
    darena::ServerIDHeightmapsResponse res = {client_id, heightmaps};
    msgpack::packer<msgpack::sbuffer> packer(&buffers[client_id]);
    packer.pack(res);
  }

  for (int i = 0; i < client_id; i++) {
    noerr = server.send_response(i, std::move(buffers[i]));
    if (!noerr) {
      return 1;
    }
  }

  int id_playing = 0;
  int id_waiting = 1;
  while (true) {
    noerr = server.get_turn_data(id_playing);
    if (!noerr) {
      return 1;
    }
    server.trim_turn_data();
    msgpack::sbuffer buf;
    msgpack::packer<msgpack::sbuffer> packer(&buf);
    packer.pack(server.turn_data);
    noerr = server.send_response(id_waiting, std::move(buf));
    if (!noerr) {
      return 1;
    }
    id_playing = id_waiting;
    id_waiting = 1 - id_playing;
  }

  server.cleanup();
  darena::log << "Server ended.\n";

  return 0;
}
