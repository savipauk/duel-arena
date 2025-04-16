#pragma once

#include <condition_variable>

#include "common.h"

namespace darena {

struct Game;

class Enemy {
 private:
  darena::Position position;
  int width;
  int height;
  int cannon_width;
  int cannon_height;
  int move_speed = 100;
  std::unique_ptr<darena::ClientTurn> current_turn_data;

  float gravity = 4.91f;
  float current_y_speed = 0.0f;
  float current_x_speed = 0.0f;
  float max_y_speed = 1000;
  float deacceleration_x = 500;

  int zero_movement_counter = 0;

  float angle_rad = 0.0f;

  float shot_angle = M_PI / 4.0f;
  float shot_angle_should_be = M_PI / 4.0f;
  float shot_angle_change_speed = 3;
  float min_shot_angle = 0.0f;
  float max_shot_angle = M_PI / 2.0f;

  float shot_power = 0.0f;
  bool falling = false;

  void simulation_thread();
  size_t movement_index = 0;
  size_t shot_angle_index = 0;
  bool shot_initiated = false;
  enum class CurrentAction { IDLE, MOVING, AIMING, SHOOTING };
  std::atomic<CurrentAction> current_action{CurrentAction::IDLE};
  std::condition_variable action_cv;  // To signal completion of an action
  std::atomic_bool action_finished{
      true};  // True if update() finished last requested step
  std::mutex simulation_mutex;
  int move_x = 0;
  int move_y = 0;

 public:
  std::vector<darena::IslandPoint> heightmap;
  std::atomic_bool is_simulating{false};

  Enemy(float x, float y, int width, int height)
      : position(x, y), width(width), height(height) {
    cannon_width = width * 1;
    cannon_height = height / 2;
  };

  void process_input(darena::Game* game, SDL_Event* e);
  void update(darena::Game* game, float delta_time);
  void render(darena::Game* game);

  void start_simulation(std::unique_ptr<darena::ClientTurn> turn_data);
};

}  // namespace darena
