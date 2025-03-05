#include "common.h"

namespace darena {

std::string Position::to_string() const {
  return "Position(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

std::string IslandPoint::to_string() const {
  return "IslandPoint(" + std::to_string(height) + ")";
}

Logger log;

}  // namespace darena
