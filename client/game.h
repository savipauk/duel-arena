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
  const char* win_by_fall = "I win. The enemy has fallen into the void.";
  const char* lose_by_fall = "I lose. I fell into the void.";
  const char* win_by_self_destruct = "I win. The enemy has self destructed.";
  const char* lose_by_self_destruct = "I lose. I self destructed.";
  const char* win_by_destroy = "I win. I destroyed the enemy.";
  const char* lose_by_destroy = "I lose. I got destroyed.";
  const char* win_by_unknown = "I win. I don't know why.";
  const char* lose_by_unknown = "I lose. I don't know why.";
  enum class GameEndWay { FALL, SELF_DESTRUCT, DESTROY };
  const char** game_end_message = &win_by_fall;
  bool game_end = false;
  bool game_win = false;

  int id;
  bool my_turn;
  bool enemy_was_simulating_previous_step = false;
  bool check_for_enemy_finished = false;
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

  // Ends the gameturn
  void end_game(bool win, GameEndWay how);

  // Resets the projectile pointer and calls send_turn_data if its my turn
  void projectile_hit();

  // Sends turn data to the server
  void send_turn_data();

  // Simulates the turn
  bool simulate_turn();

  // Simulates enemy shooting
  bool simulate_enemy_shoot();

  // Waits for the server to send other player turn data
  bool get_turn_data();

  // Update functions
  void process_input(SDL_Event* e);
  void update(float delta_time);
  void render();

  // Placeholder function to test functionality
  std::vector<darena::IslandPoint> generate(const Vec2& position,
                                            int num_of_points);
};

}  // namespace darena
