#include "enemy.h"

#include <SDL_opengl.h>

#include <mutex>
#include <thread>

#include "game.h"

namespace darena {

void Enemy::process_input(darena::Game* game, SDL_Event* e) {}

void Enemy::simulation_thread() {
  if (!current_turn_data) {
    darena::log << "current_turn_data not set!\n";
    return;
  }

  while (movement_index < current_turn_data->movements.size()) {
    {
      // Wait for update() to be ready
      std::unique_lock lock(simulation_mutex);
      action_cv.wait(lock, [this] { return action_finished.load(); });

      current_action = CurrentAction::MOVING;
      move_x = current_turn_data->movements[movement_index];

      // Signal to update() that it has a new action
      action_finished = false;
    }
    {
      // Wait for update() to finish current action
      std::unique_lock lock(simulation_mutex);
      action_cv.wait(lock, [this] { return action_finished.load(); });
      movement_index++;
    }
  }

  while (!are_equal(position.x, current_turn_data->final_position.x)) {
    position.x = current_turn_data->final_position.x;
  }

  // Wait 1 second
  usleep(1000 * 1000);

  while (shot_angle_index < current_turn_data->angle_changes.size()) {
    {
      std::unique_lock lock(simulation_mutex);
      action_cv.wait(lock, [this] { return action_finished.load(); });

      current_action = CurrentAction::AIMING;
      move_y = current_turn_data->angle_changes[shot_angle_index];

      action_finished = false;
    }
    {
      std::unique_lock lock(simulation_mutex);
      action_cv.wait(lock, [this] { return action_finished.load(); });
      shot_angle_index++;
    }
  }

  if (!shot_initiated) {
    {
      std::unique_lock lock(simulation_mutex);
      action_cv.wait(lock, [this] { return action_finished.load(); });

      current_action = CurrentAction::SHOOTING;
      shot_power = current_turn_data->shot_power;
      shot_angle_should_be = current_turn_data->shot_angle;

      action_finished = false;
    }
    {
      std::unique_lock lock(simulation_mutex);
      action_cv.wait(lock, [this] { return action_finished.load(); });
      shot_initiated = true;
    }
  }

  // Simulation complete
  current_action = CurrentAction::IDLE;
  is_simulating = false;
  current_turn_data.reset();
}

void Enemy::start_simulation(std::unique_ptr<darena::ClientTurn> turn_data) {
  if (is_simulating.load()) {
    darena::log << "Already simulating enemy movement!\n";
  }

  current_turn_data = std::move(turn_data);
  current_action = CurrentAction::IDLE;
  movement_index = 0;
  shot_angle_index = 0;
  shot_initiated = false;
  action_finished.store(true);
  is_simulating.store(true);

  std::thread([this]() { this->simulation_thread(); }).detach();
}

void Enemy::update(darena::Game* game, float delta_time) {
  if (falling) {
    current_y_speed += gravity * FIXED_TIMESTEP;
    if (current_y_speed >= max_y_speed) {
      current_y_speed = max_y_speed;
    }
    position.y += current_y_speed;
    if (position.y >= WINDOW_HEIGHT && !game->my_turn) {
      shot_angle = 0;
      shot_power = -1;
      lost = true;
    }
  } else {
    current_y_speed = 0;
  }

  auto closest_it = heightmap->begin();
  float closest_distance =
      ISLAND_POINT_EVERY;  // If >= ISLAND_POINT_EVERY / 2 then the player is
                           // off the island
  for (auto it = heightmap->begin(); it != heightmap->end(); it++) {
    Position point = it->position;
    float distance = point.x - position.x;
    if (std::fabs(distance) < std::fabs(closest_distance)) {
      closest_it = it;
      closest_distance = distance;
    }
  }

  Position closest = closest_it->position;
  Position snd_closest = closest;
  if (closest_distance < 0 && closest_it != heightmap->begin()) {
    snd_closest = (*(--closest_it)).position;
  } else if (closest_distance > 0 &&
             std::next(closest_it) != heightmap->end()) {
    snd_closest = (*(++closest_it)).position;
  }
  // Else closest_distance is 0 (exactly in the middle of a point), or player is
  // falling

  if (position.y + height / 2.0f < closest.y ||
      closest_distance > ISLAND_POINT_EVERY / 2.0f ||
      closest.y >= (float)(ISLAND_Y_OFFSET + ISLAND_HEIGHT)) {
    falling = true;
  } else {
    falling = false;
  }

  float slope = 0.0f;
  if (!are_equal(closest.x, snd_closest.x) && !falling) {
    slope = (snd_closest.y - closest.y) / (snd_closest.x - closest.x);
    angle_rad = std::atan(slope) / 2.0f;
  } else {
    angle_rad = 0.0f;
  }

  if (!falling && !are_equal(slope, 0.0f)) {
    float y_intercept = closest.y - slope * closest.x;
    float y_point = slope * position.x + y_intercept;
    position.y = std::round(y_point) - height / 2.0f + 5;
  }

  if (is_simulating.load() && !action_finished.load()) {
    bool finished_frame = false;
    CurrentAction action = current_action.load();
    switch (action) {
      case CurrentAction::MOVING: {
        darena::log << "Moving\n";
        if (move_x != 0) {
          current_x_speed = move_x * move_speed;
          zero_movement_counter = 0;
        } else {
          if (are_equal(current_x_speed, 0.0f)) {
            current_x_speed = 0;
          } else {
            int multiplier = 1;
            if (current_x_speed < 0) {
              multiplier = -1;
            }

            current_x_speed -= multiplier * deacceleration_x * FIXED_TIMESTEP;
            zero_movement_counter++;
            // current_x_speed = 0;
            if (zero_movement_counter >= MAX_N_OF_ZERO_IN_MOVEMENT) {
              current_x_speed = 0;
            }
          }
        }
        position.x += current_x_speed * FIXED_TIMESTEP;
        finished_frame = true;
        break;
      }
      case CurrentAction::AIMING: {
        darena::log << "Aiming\n";
        shot_angle += move_y * shot_angle_change_speed * FIXED_TIMESTEP;
        shot_angle = std::clamp(shot_angle, min_shot_angle, max_shot_angle);
        finished_frame = true;
        break;
      }
      case CurrentAction::SHOOTING: {
        darena::log << "Shooting\n";

        int shot_direction = 1;
        if (game->id == 0) {
          shot_direction = -1;
        }

        if (!shot) {
          game->projectile = std::make_unique<darena::Projectile>(
              position.x, position.y, shot_angle, shot_power, shot_direction);
          shot = true;
        }

        if (game->projectile == nullptr) {
          shot = false;
          finished_frame = true;
        }
        break;
      }
      case CurrentAction::IDLE: {
        darena::log << "Idle\n";
        finished_frame = true;
        break;
      }
    }

    if (finished_frame) {
      {
        std::lock_guard lock(simulation_mutex);
        action_finished = true;
        current_action = CurrentAction::IDLE;
      }
      action_cv.notify_one();
    }
  }
}

void Enemy::render(darena::Game* game) {
  // Enemy
  int angle_deg = angle_rad * (180.0f / M_PI);
  Position top_left = {-width / 2.0f, height / 2.0f};
  Position top_right = {width / 2.0f, height / 2.0f};
  Position bot_right = {width / 2.0f, -height / 2.0f};
  Position bot_left = {-width / 2.0f, -height / 2.0f};

  glPushMatrix();
  glTranslatef(position.x, position.y, 0);
  glRotatef(angle_deg, 0, 0, 1);

  if (game->id == 1) {
    glColor3f(0.3f, 0.5f, 1.0f);
  } else {
    glColor3f(1.0f, 0.5f, 0.3f);
  }
  glBegin(GL_POLYGON);

  glVertex2f(top_left.x, top_left.y);
  glVertex2f(top_right.x, top_right.y);
  glVertex2f(bot_right.x, bot_right.y);
  glVertex2f(bot_left.x, bot_left.y);

  glEnd();
  glPopMatrix();

  // Cannon
  int cannon_angle_deg = shot_angle * (180.f / M_PI);
  top_left = {-cannon_width / 2.0f, cannon_height / 2.0f};
  top_right = {cannon_width / 2.0f, cannon_height / 2.0f};
  bot_right = {cannon_width / 2.0f, -cannon_height / 2.0f};
  bot_left = {-cannon_width / 2.0f, -cannon_height / 2.0f};

  glPushMatrix();
  if (game->id == 1) {
    // Move to the pivot point (player center)
    glTranslatef(position.x, position.y, 0);
    // Rotate
    glRotatef(-cannon_angle_deg, 0, 0, 1);
    // Move it to the intended positiion
    glTranslatef(cannon_width / 2.0, 0, 0);
  } else {
    // Move to the pivot point (player center)
    glTranslatef(position.x, position.y, 0);
    // Rotate
    glRotatef(cannon_angle_deg, 0, 0, 1);
    // Move it to the intended positiion
    glTranslatef(-cannon_width / 2.0, 0, 0);
  }

  if (game->id == 1) {
    glColor3f(0.1f, 0.3f, 0.8f);
  } else {
    glColor3f(0.8f, 0.3f, 0.1f);
  }
  glBegin(GL_POLYGON);

  glVertex2f(top_left.x, top_left.y);
  glVertex2f(top_right.x, top_right.y);
  glVertex2f(bot_right.x, bot_right.y);
  glVertex2f(bot_left.x, bot_left.y);

  glEnd();
  glPopMatrix();

  // Shot power bar
  int bar_width = width * 2;
  int bar_height = height / 2;
  top_left = {-bar_width / 2.0f, bar_height / 2.0f};
  top_right = {bar_width / 2.0f, bar_height / 2.0f};
  bot_right = {bar_width / 2.0f, -bar_height / 2.0f};
  bot_left = {-bar_width / 2.0f, -bar_height / 2.0f};

  glPushMatrix();

  glTranslatef(position.x, position.y - 50, 0);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_LINE_LOOP);

  glVertex2f(top_left.x, top_left.y);
  glVertex2f(top_right.x, top_right.y);
  glVertex2f(bot_right.x, bot_right.y);
  glVertex2f(bot_left.x, bot_left.y);

  glEnd();

  float bar_shot_power = shot_power;
  if (are_equal(shot_power, -1)) {
    bar_shot_power = 0;
  }
  float percentage_filled = bar_shot_power / 100.0;
  float diff = top_right.x - top_left.x;
  top_right.x -= diff * (1 - percentage_filled);
  bot_right.x -= diff * (1 - percentage_filled);

  glBegin(GL_POLYGON);

  glVertex2f(top_left.x, top_left.y);
  glVertex2f(top_right.x, top_right.y);
  glVertex2f(bot_right.x, bot_right.y);
  glVertex2f(bot_left.x, bot_left.y);

  glEnd();
  glPopMatrix();
}

}  // namespace darena
