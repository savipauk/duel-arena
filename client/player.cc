#include "player.h"

#include <SDL_opengl.h>

#include "game.h"
#include "imgui.h"

namespace darena {

void Player::process_input(darena::Game* game, SDL_Event* e) {
  bool last_frame_space = keys_pressed.count(SDLK_SPACE);
  move_x = 0;
  move_y = 0;

  switch (e->type) {
    case SDL_KEYDOWN: {
      keys_pressed.insert(e->key.keysym.sym);
      break;
    }
    case SDL_KEYUP: {
      keys_pressed.erase(e->key.keysym.sym);
      break;
    }
  }

  move_x -= keys_pressed.count(SDLK_LEFT);
  move_x += keys_pressed.count(SDLK_RIGHT);

  move_y += keys_pressed.count(SDLK_UP);
  move_y -= keys_pressed.count(SDLK_DOWN);

  // Shooting
  bool change_happened = last_frame_space != keys_pressed.count(SDLK_SPACE);
  if (keys_pressed.count(SDLK_SPACE) && change_happened) {
    switch (shot_state) {
      case IDLE:
        // Initiated shot
        shot_state = CHARGING;
        darena::log << "IDLE -> CHARGING\n";
        break;
      case CHARGING:
        // Decided to shoot
        shot_state = SHOOT;
        darena::log << "CHARGING -> SHOOT\n";
        break;
      case SHOOT:
        break;
    }
  }
}

void Player::update(darena::Game* game, float delta_time) {
  if (falling) {
    move_x = 0;
    current_y_speed += gravity * delta_time;
    if (current_y_speed >= max_y_speed) {
      current_y_speed = max_y_speed;
    }
    position.y += current_y_speed;
  } else {
    current_y_speed = 0;
  }

  auto closest_it = game->left_island->heightmap.begin();
  float closest_distance =
      ISLAND_POINT_EVERY;  // If >= ISLAND_POINT_EVERY / 2 then the player is
                           // off the island
  for (auto it = game->left_island->heightmap.begin();
       it != game->left_island->heightmap.end(); it++) {
    Position point = it->position;
    float distance = point.x - position.x;
    if (std::fabs(distance) < std::fabs(closest_distance)) {
      closest_it = it;
      closest_distance = distance;
    }
  }

  Position closest = closest_it->position;
  Position snd_closest = closest;
  if (closest_distance < 0 &&
      closest_it != game->left_island->heightmap.begin()) {
    snd_closest = (*(--closest_it)).position;
  } else if (closest_distance > 0 &&
             std::next(closest_it) != game->left_island->heightmap.end()) {
    snd_closest = (*(++closest_it)).position;
  }
  // Else closest_distance is 0 (exactly in the middle of a point), or player is
  // falling

  if (position.y + width / 2.0f < closest.y ||
      closest_distance > ISLAND_POINT_EVERY / 2.0f) {
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

  // if (!are_equal(slope, 0.0f)) {
  if (!falling && !are_equal(slope, 0.0f)) {
    float y_intercept = closest.y - slope * closest.x;
    float y_point = slope * position.x + y_intercept;
    position.y = std::round(y_point) - width / 2.0f + 5;
  }

  if (move_x != 0) {
    current_x_speed = move_x * move_speed;
  } else if (move_x == 0 || falling) {
    if (are_equal(current_x_speed, 0.0f)) {
      current_x_speed = 0;
    } else {
      int multiplier = 1;
      if (current_x_speed < 0) {
        multiplier = -1;
      }
      current_x_speed -= multiplier * deacceleration_x * delta_time;
    }
  }

  switch (shot_state) {
    case IDLE: {
      position.x += current_x_speed * delta_time;
      shot_angle += move_y * shot_angle_change_speed * delta_time;
      shot_angle = std::clamp(shot_angle, min_shot_angle, max_shot_angle);
      break;
    }
    case CHARGING: {
      shot_power += move_y * shot_power_change_speed * delta_time;
      shot_power = std::clamp(shot_power, min_shot_power, max_shot_power);
      break;
    }
    case SHOOT: {
      break;
    }
  }


}

void Player::render(darena::Game* game) {
  // Player
  int angle_deg = angle_rad * (180.0f / M_PI);
  Position top_left = {-width / 2.0f, height / 2.0f};
  Position top_right = {width / 2.0f, height / 2.0f};
  Position bot_right = {width / 2.0f, -height / 2.0f};
  Position bot_left = {-width / 2.0f, -height / 2.0f};

  glPushMatrix();
  glTranslatef(position.x, position.y, 0);
  glRotatef(angle_deg, 0, 0, 1);

  glColor3f(0.3f, 0.5f, 1.0f);
  glBegin(GL_POLYGON);

  glVertex2f(top_left.x, top_left.y);
  glVertex2f(top_right.x, top_right.y);
  glVertex2f(bot_right.x, bot_right.y);
  glVertex2f(bot_left.x, bot_left.y);

  glEnd();
  glPopMatrix();

  // Cannon
  int cannon_angle_deg = shot_angle * (180.f / M_PI);
  int cannon_width = width * 1;
  int cannon_height = height / 2;
  top_left = {-cannon_width / 2.0f, cannon_height / 2.0f};
  top_right = {cannon_width / 2.0f, cannon_height / 2.0f};
  bot_right = {cannon_width / 2.0f, -cannon_height / 2.0f};
  bot_left = {-cannon_width / 2.0f, -cannon_height / 2.0f};

  glPushMatrix();
  // Move to the pivot point (player center)
  glTranslatef(position.x, position.y, 0);
  // Rotate
  glRotatef(-cannon_angle_deg, 0, 0, 1);
  // Move it to the intended positiion
  glTranslatef(cannon_width / 2.0, 0, 0);

  glColor3f(0.1f, 0.3f, 0.8f);
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
  
  float percentage_filled = shot_power / 100.0;
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
  

  // Shot power text
  ImGui::SetNextWindowPos(ImVec2(50, 50));

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoSavedSettings;

  ImGui::Begin("Overlay", nullptr, window_flags);
  ImGui::Text("Shot power: %0.f  ",
              shot_power);  // Two whitespaces because for some reason ImGui
                            // doesn't size the text box properly
  ImGui::End();
}

}  // namespace darena
