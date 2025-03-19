#include "game.h"

#include <SDL_opengl.h>

#include <random>

#include "client_lib.h"

namespace darena {

void Game::set_state(std::unique_ptr<GameState> new_state) {
  state = std::move(new_state);
}

bool Game::connect_to_server() {
  bool noerr;

  darena::log << "Connecting to server " << server_ip << " with username "
              << username << "\n";
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
  glBegin(GL_POLYGON);
  // Bottom left point first
  glVertex2i(left_island->heightmap[0].position.x,
             ISLAND_Y_OFFSET + ISLAND_HEIGHT);
  for (const darena::IslandPoint& point : left_island->heightmap) {
    glVertex2i(point.position.x, point.position.y);
  }
  // Bottom right point last
  glVertex2i(left_island->heightmap.back().position.x,
             ISLAND_Y_OFFSET + ISLAND_HEIGHT);
  glEnd();

  // Draw the right island
  glBegin(GL_POLYGON);
  // Bottom left point first
  glVertex2i(right_island->heightmap[0].position.x,
             ISLAND_Y_OFFSET + ISLAND_HEIGHT);
  for (const darena::IslandPoint& point : right_island->heightmap) {
    glVertex2i(point.position.x, point.position.y);
  }
  // Bottom right point last
  glVertex2i(right_island->heightmap.back().position.x,
             ISLAND_Y_OFFSET + ISLAND_HEIGHT);
  glEnd();
}

// Used to randomly generate numbers in generate_heightmap()
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

std::vector<darena::IslandPoint> Game::generate(
    const Position& starting_position, int num_of_points) {
  std::vector<darena::IslandPoint> output = {};
  int random_strength = 1;
  int last_height = 50 + (dis(gen) - 0.5) * 50;
  float x = starting_position.x;
  float y = starting_position.y;
  for (int i = 0; i < num_of_points; i++) {
    last_height += (dis(gen) - 0.5) * 10;

    if (last_height < 25) {
      last_height = 25;
    } else if (last_height > 75) {
      last_height = 75;
    }

    y = starting_position.y + last_height;
    Position position{x, y};
    output.emplace_back(position, random_strength);
    x += ISLAND_WIDTH / ISLAND_NUM_OF_POINTS;
  }

  return output;
}

}  // namespace darena
