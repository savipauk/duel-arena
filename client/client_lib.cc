#include "client_lib.h"

namespace darena {

std::string Island::to_string() const {
  std::string output = "Island(" + position.to_string();
  for (auto it = height_map.begin(); it != height_map.end(); it++) {
    output.append(it->to_string());
  }
  output.append(")");
  return output;
}

}  // namespace darena
