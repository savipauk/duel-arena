#pragma once

#include "common.h"

namespace darena {

struct Game;

class Enemy {
 private:
 public:
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

  float angle_rad = 0.0f;
  float shot_angle = 0.0f;
  float shot_power = 0.0f;
  bool falling = false;

  std::vector<darena::IslandPoint> heightmap;

  Enemy(float x, float y, int width, int height)
      : position(x, y), width(width), height(height) {
    cannon_width = width * 1;
    cannon_height = height / 2;
  };

  void process_input(darena::Game* game, SDL_Event* e);
  void update(darena::Game* game, float delta_time);
  void render(darena::Game* game);
};

}  // namespace darena
