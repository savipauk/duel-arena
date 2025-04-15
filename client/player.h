#pragma once

#include <SDL_events.h>

#include "common.h"

namespace darena {

struct Game;

class Player {
 private:
  std::unordered_set<SDL_Keycode> keys_pressed;
  int move_x = 0;
  int move_y = 0;

 public:
  enum ShotState { IDLE, CHARGING, SHOOT };
  ShotState shot_state = IDLE;
  darena::Position position;
  int width;
  int height;
  int cannon_width;
  int cannon_height;
  int move_speed = 100;
  float gravity = 4.91f;
  float current_y_speed = 0.0f;
  float current_x_speed = 0.0f;
  float max_y_speed = 1000;
  float deacceleration_x = 500;
  bool falling = false;
  float angle_rad = 0.0f;

  std::vector<darena::IslandPoint> heightmap;

  float shot_angle = M_PI / 4.0f;
  float shot_angle_change_speed = 3;
  float min_shot_angle = 0.0f;
  float max_shot_angle = M_PI / 2.0f;

  float shot_power = 0.0f;
  float shot_power_change_speed = 35;
  float min_shot_power = 0.0f;
  float max_shot_power = 100;

  Player(float x, float y, int width, int height)
      : position(x, y), width(width), height(height) {
    cannon_width = width * 1;
    cannon_height = height / 2;
  };

  void process_input(darena::Game* game, SDL_Event* e);
  void update(darena::Game* game, float delta_time);
  void render(darena::Game* game);

  void reset();
};

}  // namespace darena
