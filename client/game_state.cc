#include "game_state.h"

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

void GSConnected::process_input(Game* game, SDL_Event* e) { return; }

void GSConnected::update(Game* game, float delta_time) {}

void GSConnected::render(Game* game) { game->draw_islands(); }

}  // namespace darena
