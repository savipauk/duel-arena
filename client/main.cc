#include <SDL.h>
#include <SDL_net.h>

#include "engine.h"

int main() {
  try {
    darena::Engine engine;
    bool noerr = engine.run();
    if (!noerr) {
      engine.cleanup();
      return 1;
    }
    engine.cleanup();
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Uncaught exception: " << e.what() << "\n";
  } catch (...) {
    std::cerr << "Unknown uncaught exception!\n";
  }
}
