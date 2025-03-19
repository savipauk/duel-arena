#include "game_state.h"

#include <SDL_opengl.h>

#include "game.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_stdlib.h"

namespace darena {

void GSInitial::process_input(Game* game, SDL_Event* e) { return; }

void GSInitial::update(Game* game, float delta_time) { return; }

void GSInitial::render(Game* game) {
  // Render username control in the middle
  ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
  ImVec2 window_pos = ImVec2(viewport_size.x * 0.5f, viewport_size.y * 0.5f);
  ImVec2 window_size = ImVec2(WINDOW_WIDTH * 0.35f, WINDOW_HEIGHT * 0.25f);
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(window_size);

  ImGui::Begin("INPUT", nullptr,
               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

  ImGui::InputText("Username", &game->username);
  ImGui::InputText("Server IP", &game->server_ip);
  bool button = ImGui::Button("Connect");

  if (button) {
    game->set_state(std::make_unique<GSConnecting>());
  }

  ImGui::End();
}

void GSConnecting::process_input(Game* game, SDL_Event* e) { return; }

void GSConnecting::job(Game* game) {
  bool successfully_connected = game->connect_to_server();
  if (successfully_connected) {
    thread_running = false;
    game->set_state(std::make_unique<GSWaitingForIslandData>());
  }
}
void GSConnecting::update(Game* game, float delta_time) {
  if (!thread_running) {
    network_thread = std::thread([this, game]() { this->job(game); });
    network_thread.detach();
    thread_running = true;
  }
}

void GSConnecting::render(Game* game) { ImGui::Text("CONNECTING"); }

void GSWaitingForIslandData::process_input(Game* game, SDL_Event* e) { return; }

void GSWaitingForIslandData::job(Game* game) {
  bool successfully_connected = game->get_island_data();
  if (successfully_connected) {
    thread_running = false;
    game->set_state(std::make_unique<GSConnected>());
  }
}

void GSWaitingForIslandData::update(Game* game, float delta_time) {
  if (!thread_running) {
    network_thread = std::thread([this, game]() { this->job(game); });
    network_thread.detach();
    thread_running = true;
  }
}

void GSWaitingForIslandData::render(Game* game) {
  ImGui::Text("WAITING FOR GAME TO START");
}

void GSConnected::process_input(Game* game, SDL_Event* e) {
  move_x = 0;

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

  if (keys_pressed.count(SDLK_r)) {
    game->player.position = {100, 100};
  }
  if (keys_pressed.count(SDLK_n)) {
    generated = false;
    game->player.position = {100, 100};
  }
}

bool are_equal(float x1, float x2, float epsilon = 1e-10) {
  return std::fabs(x1 - x2) < epsilon;
}

void GSConnected::update(Game* game, float delta_time) {
  if (!generated) {
    Position left_island_starting_position{ISLAND_X_OFFSET, ISLAND_Y_OFFSET};
    Position right_island_starting_position{
        WINDOW_WIDTH - ISLAND_X_OFFSET - ISLAND_WIDTH, ISLAND_Y_OFFSET};
    game->left_island->heightmap =
        game->generate(left_island_starting_position, ISLAND_NUM_OF_POINTS);
    game->right_island->heightmap =
        game->generate(right_island_starting_position, ISLAND_NUM_OF_POINTS);
    generated = true;
    return;
  }

  Player* player = &game->player;

  if (player->falling) {
    move_x = 0;
    player->current_y_speed += player->gravity * delta_time;
    if (player->current_y_speed >= player->max_y_speed) {
      player->current_y_speed = player->max_y_speed;
    }
    player->position.y += player->current_y_speed;
  } else {
    player->current_y_speed = 0;
  }

  auto closest_it = game->left_island->heightmap.begin();
  float closest_distance =
      ISLAND_POINT_EVERY;  // If >= ISLAND_POINT_EVERY / 2 then the player is
                           // off the island
  for (auto it = game->left_island->heightmap.begin();
       it != game->left_island->heightmap.end(); it++) {
    Position point = it->position;
    float distance = point.x - player->position.x;
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

  if (player->position.y + player->width / 2.0f < closest.y ||
      closest_distance > ISLAND_POINT_EVERY / 2.0f) {
    player->falling = true;
  } else {
    player->falling = false;
  }

  float slope = 0.0f;
  if (!are_equal(closest.x, snd_closest.x) && !player->falling) {
    slope = (snd_closest.y - closest.y) / (snd_closest.x - closest.x);
    player->angle_rad = std::atan(slope) / 2.0f;
  } else {
    player->angle_rad = 0.0f;
  }

  // if (!are_equal(slope, 0.0f)) {
  if (!player->falling && !are_equal(slope, 0.0f)) {
    float y_intercept = closest.y - slope * closest.x;
    float y_point = slope * player->position.x + y_intercept;
    player->position.y = std::round(y_point) - player->width / 2.0f + 5;
  }

  if (move_x != 0) {
    player->current_x_speed = move_x * player->move_speed;
  } else if (move_x == 0 || player->falling) {
    if (are_equal(player->current_x_speed, 0.0f)) {
      player->current_x_speed = 0;
    } else {
      int multiplier = 1;
      if (player->current_x_speed < 0) {
        multiplier = -1;
      }
      player->current_x_speed -=
          multiplier * player->deacceleration_x * delta_time;
    }
  }

  player->position.x += player->current_x_speed * delta_time;
}

void GSConnected::render(Game* game) {
  if (!generated) {
    return;
  }

  game->draw_islands();

  Player* player = &game->player;
  int angle_deg = player->angle_rad * (180.0f / M_PI);

  glPushMatrix();
  glTranslatef(player->position.x, player->position.y, 0);
  glRotatef(angle_deg, 0, 0, 1);

  glColor3f(0.3f, 0.5f, 1.0f);
  glBegin(GL_POLYGON);

  glVertex2f(-player->width / 2.0, player->height / 2.0);
  glVertex2f(player->width / 2.0, player->height / 2.0);
  glVertex2f(player->width / 2.0, -player->height / 2.0);
  glVertex2f(-player->width / 2.0, -player->height / 2.0);

  glEnd();

  glPopMatrix();
}

}  // namespace darena
