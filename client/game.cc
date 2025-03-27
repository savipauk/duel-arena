#include "game.h"

#include <SDL_opengl.h>

#include <random>

#include "client_lib.h"
#include "common.h"

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
  darena::ServerIDHeightmapsResponse res;
  obj.convert(res);
  id = res.client_id;
  if (id == 0) {
    my_turn = true;
  } else {
    my_turn = false;
  }
  left_island = std::make_unique<darena::Island>(left_island_starting_position,
                                                 res.heightmaps[0]);
  right_island = std::make_unique<darena::Island>(
      right_island_starting_position, res.heightmaps[1]);

  return true;
}

void Game::process_input(SDL_Event* e) {
  state->process_input(this, e);

  if (my_turn) {
    if (player) {
      player->process_input(this, e);
    }
  }

  if (enemy) {
    enemy->process_input(this, e);
  }

  if (left_island) {
    left_island->process_input(this, e);
  }

  if (right_island) {
    right_island->process_input(this, e);
  }
}

void Game::update(float delta_time) {
  state->update(this, delta_time);

  if (player) {
    player->update(this, delta_time);
  }

  if (enemy) {
    enemy->update(this, delta_time);
  }

  if (left_island) {
    left_island->update(this, delta_time);
  }

  if (right_island) {
    right_island->update(this, delta_time);
  }
}

void Game::render() {
  // Draw order matters!

  if (left_island) {
    left_island->render(this);
  }

  if (right_island) {
    right_island->render(this);
  }

  if (player) {
    player->render(this);
  }

  if (enemy) {
    enemy->render(this);
  }

  state->render(this);
}

// PLACEHOLDER FUNCTIONS FOR TESTING
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

void update(Game* game, float delta_time) {}

}  // namespace darena
