#include "game.h"

#include <SDL_opengl.h>

#include "client_lib.h"

namespace darena {

void Game::set_state(std::unique_ptr<GameState> new_state) {
  state = std::move(new_state);
}

bool Game::connect_to_server() {
  bool noerr;

  darena::log << "Connecting to server " << server_ip << " with username " << username << "\n";
  client.username = username;
  client.server_ip_string = server_ip;

  noerr = client.initialize();
  if (!noerr) {
    return false;
  }

  noerr = client.send_connection_request();
  if (!noerr) {
    return false;
  }

  return true;
}

bool Game::get_island_data() {
  bool noerr = client.wait_for_message();
  if (!noerr) {
    return false;
  }

  std::optional<msgpack::unpacked> response = client.get_connection_response();
  if (!response.has_value()) {
    return false;
  }

  msgpack::object obj = response->get();
  std::vector<std::vector<darena::IslandPoint>> heightmaps;
  obj.convert(heightmaps);
  left_island->heightmap = heightmaps[0];
  right_island->heightmap = heightmaps[1];

  return true;
}

void Game::draw_islands() {
  glColor3f(1.0f, 1.0f, 1.0f);

  // Draw the left island
  glBegin(GL_LINE_STRIP);
  int x = ISLAND_X_OFFSET;
  for (const darena::IslandPoint& point : left_island->heightmap) {
    int y = ISLAND_Y_OFFSET + point.height;
    glVertex2i(x, y);
    x += ISLAND_WIDTH / ISLAND_NUM_OF_POINTS;
  }
  glEnd();

  // Draw the right island
  x = WINDOW_WIDTH - ISLAND_X_OFFSET;
  glBegin(GL_LINE_STRIP);
  for (const darena::IslandPoint& point : right_island->heightmap) {
    int y = ISLAND_Y_OFFSET + point.height;
    glVertex2i(x, y);
    x -= ISLAND_WIDTH / ISLAND_NUM_OF_POINTS;
  }
  glEnd();
}

}  // namespace darena
