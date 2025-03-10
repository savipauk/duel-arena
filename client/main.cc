#include <SDL.h>
#include <SDL_net.h>

#include "engine.h"

int main() {
  darena::Engine engine;

  bool noerr = engine.initialize(); 
  if (!noerr) {
    return 1;
  }

  noerr = engine.run();

  if (!noerr) {
    engine.cleanup();
    return 1;
  }

  engine.cleanup();
  return 0;
}
