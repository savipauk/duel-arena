#pragma once

#include <SDL_events.h>

#include <atomic>

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
  std::atomic_bool thread_running;
  std::atomic_bool transition_ready;
  void job(Game* game);

 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

class GSWaitingForIslandData : public GameState {
 private:
  std::atomic_bool thread_running;
  std::atomic_bool transition_ready;
  void job(Game* game);

 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

class GSConnected : public GameState {
 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

class GSPlayTurn : public GameState {
 private:
  bool reset = false;

 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

class GSShootProjectile : public GameState {
 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

class GSWaitTurn : public GameState {
 private:
  std::atomic_bool thread_running;
  std::atomic_bool transition_ready;
  void job(Game* game);

 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

class GSSimulateTurn : public GameState {
 private:
  bool sent = false;

 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

class GSWonGame : public GameState {
 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

class GSLoseGame : public GameState {
 public:
  void process_input(darena::Game* game, SDL_Event* e) override;
  void update(darena::Game* game, float delta_time) override;
  void render(darena::Game* game) override;
};

}  // namespace darena
