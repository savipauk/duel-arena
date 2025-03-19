#pragma once

#include <SDL_events.h>

#include <thread>
#include <unordered_set>

namespace darena {

struct Game;

class GameState {
 public:
  virtual ~GameState() = default;
  virtual void process_input(darena::Game* game, SDL_Event* e) = 0;
  virtual void update(darena::Game* game, float delta_time) = 0;
  virtual void render(darena::Game* game) = 0;
};

class GSInitial : public GameState {
 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

class GSConnecting : public GameState {
 private:
  std::thread network_thread;
  std::atomic_bool thread_running;
  void job(Game* game);

 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

class GSWaitingForIslandData : public GameState {
 private:
  std::thread network_thread;
  std::atomic_bool thread_running;
  void job(Game* game);

 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

class GSConnected : public GameState {
 private:
  bool generated = false;
  std::unordered_set<SDL_Keycode> keys_pressed;
  int move_x = 0;

 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

}  // namespace darena
