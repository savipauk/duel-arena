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
  enum ShotState { IDLE, CHARGING, SHOOT };
  ShotState shot_state = IDLE;

 public:
  darena::Position position;
  int width;
  int height;
  int move_speed = 100;
  float gravity = 4.91f;
  float current_y_speed = 0.0f;
  float current_x_speed = 0.0f;
  float max_y_speed = 1000;
  float deacceleration_x = 500;
  bool falling = false;
  float angle_rad = 0.0f;

  float min_shot_angle = 0.0f;
  float max_shot_angle = M_PI / 2.0f;
  float shot_angle =  M_PI / 4.0f;
  float shot_angle_change_speed = 3;
  float shot_power = 0.0f;

  Player(float x, float y, int width, int height)
      : position(x, y), width(width), height(height) {};

  void process_input(darena::Game* game, SDL_Event* e);
  void update(darena::Game* game, float delta_time);
  void render(darena::Game* game);
};

}  // namespace darena
