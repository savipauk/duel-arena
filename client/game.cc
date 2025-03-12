#include "game.h"
#include <SDL_opengl.h>

#include "client_lib.h"

namespace darena {

bool Game::connect_to_server() {
  bool noerr;

  darena::TCPClient client{server_ip, username};

  noerr = client.initialize();
  if (!noerr) {
    return false;
  }

  noerr = client.send_connection_request();
  if (!noerr) {
    return false;
  }

  std::optional<msgpack::object> response = client.get_connection_response();
  if (!response.has_value()) {
    return false;
  }

  // darena::TCPMessage message;
  // response->convert(message);
  // darena::log << "id: " << message.id << "\tmsg: " << message.data << "\n";

  response->convert(left_island->heightmap);
  for (const darena::IslandPoint point : left_island->heightmap) {
    darena::log << point.to_string() << "\n";
  }

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
