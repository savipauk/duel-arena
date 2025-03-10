#include "common.h"

#include <SDL2/SDL_net.h>

#include <sstream>

namespace darena {

std::string Position::to_string() const {
  return "Position(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

std::string IslandPoint::to_string() const {
  return "IslandPoint(" + std::to_string(height) + ")";
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

Logger log;

}  // namespace darena
