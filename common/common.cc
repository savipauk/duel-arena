#include "common.h"

#include <SDL2/SDL_net.h>

#include <sstream>

namespace darena {

std::string Vec2::to_string() const {
  return "Position(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

std::string IslandPoint::to_string() const {
  return "IslandPoint(" + position.to_string() + ", " +
         std::to_string(strength) + ")";
}

std::string unit32_t_address_to_string(uint32_t address) {
  std::ostringstream oss;

  oss << std::to_string(address >> 24) << "."
      << std::to_string((address >> 16) & 0xff) << "."
      << std::to_string((address >> 8) & 0xff) << "."
      << std::to_string(address & 0xff) << ":";

  return oss.str();
}

std::string ipaddress_to_string(IPaddress* address) {
  uint32_t ip = SDL_SwapBE32(address->host);
  std::string host = unit32_t_address_to_string(ip);
  std::string port = std::to_string(address->port);

  std::ostringstream oss;

  oss << host << ":" << port;

  return oss.str();
}

bool are_equal(float x1, float x2, float epsilon) {
  return std::fabs(x1 - x2) < epsilon;
}

Logger log;

Vec2 left_island_starting_position{ISLAND_X_OFFSET, ISLAND_Y_OFFSET};
Vec2 right_island_starting_position{
    WINDOW_WIDTH - ISLAND_X_OFFSET - ISLAND_WIDTH, ISLAND_Y_OFFSET};

}  // namespace darena
