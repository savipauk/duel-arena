#pragma once

#include "common.h"

namespace darena {

struct Game;

class Projectile {
 private:
  float gravity = 4.81f;
  float velocity_multiplier = 7;
  float initial_shot_angle = 0;
  float angle = 0;
  float velocity = 0;
  float velocity_x = 0;
  float velocity_y = 0;
  int shot_direction = 1;
  darena::Position position;
  int width = 20;
  int height = 10;
  const int max_no_hit_frames = 1;
  uint no_hit_frames_count = 0;
  bool from_a_simulation = false;

 public:
  Projectile(float x, float y, float shot_angle, float shot_power,
             float shot_direction, float from_a_simulation = false)
      : position(x, y),
        initial_shot_angle(shot_angle),
        velocity(shot_power),
        shot_direction(shot_direction),
        from_a_simulation(from_a_simulation) {
    velocity_x =
        velocity * std::cos(shot_angle) * velocity_multiplier * shot_direction;
    velocity_y = velocity * std::sin(shot_angle) * velocity_multiplier;
    angle = initial_shot_angle;
    darena::log << "shot_power: " << std::to_string(shot_power) << "\t"
                << "shot_angle: " << std::to_string(shot_angle) << "\t"
                << "velocity_x: " << std::to_string(velocity_x) << "\t"
                << "velocity_y: " << std::to_string(velocity_y) << "\n";
  }

  void hit(darena::Game* game);
  int island_hit_poll(darena::Game* game,
                      std::vector<darena::IslandPoint>& heightmap,
                      size_t check_index, float nose_x, float nose_y);
  void process_input(darena::Game* game, SDL_Event* e);
  void update(darena::Game* game, float delta_time);
  void render(darena::Game* game);
};

}  // namespace darena
