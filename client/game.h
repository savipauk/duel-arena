#pragma once

#include <memory>
#include <string>

#include "client_lib.h"
#include "enemy.h"
#include "game_state.h"
#include "island.h"
#include "player.h"
#include "projectile.h"

namespace darena {

class GameState;

struct Game {
  int id;
  bool my_turn;
  bool enemy_was_simulating_previous_step = false;
  std::string username;
  std::string server_ip;
  darena::TCPClient client;
  std::unique_ptr<darena::GameState> state;
  std::unique_ptr<darena::Player> player;
  std::unique_ptr<darena::Enemy> enemy;
  std::unique_ptr<darena::Projectile> projectile;
  std::unique_ptr<darena::Island> left_island;
  std::unique_ptr<darena::Island> right_island;
  std::unique_ptr<darena::ClientTurn> turn_data;

  Game() : client(server_ip, username) {
    state = std::make_unique<GSInitial>();
    turn_data = std::make_unique<darena::ClientTurn>();
    turn_data->movements.emplace_back(0);
    turn_data->angle_changes.emplace_back(0);
  }
  // Sets the new state
  void set_state(std::unique_ptr<GameState> new_state);

  // Connects to the server
  bool connect_to_server();

  // Waits for the server to send island data
  bool get_island_data();

  // Shoots the projectile and ends the turn 
  void end_turn();

  // Sends turn data to the server
  void send_turn_data();

  // Simulates the turn
  bool simulate_turn();

  // Waits for the server to send other player turn data
  bool get_turn_data();

  // Update functions
  void process_input(SDL_Event* e);
  void update(float delta_time);
  void render();

  // Placeholder function to test functionality
  std::vector<darena::IslandPoint> generate(const Position& position,
                                            int num_of_points);
};

}  // namespace darena
