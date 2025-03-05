#pragma once

#include <iostream>

namespace darena {

// Handles log for client/server specific prefixes.
class Logger {
 public:
#ifdef CLIENT
  std::ostream& operator<<(const std::string& message) {
    return std::cout << "CLIENT: " << message;
  }
#elif defined(SERVER)
  std::ostream& operator<<(const std::string& message) {
    return std::cout << "SERVER: " << message;
  }
#else
  std::ostream& operator<<(const std::string& message) {
    return std::cout << message;
  }
#endif

  template <typename T>
  std::ostream& operator<<(const T& value) {
    return std::cout << value;
  }
};

struct Position {
  int x;
  int y;
  Position(int x = 0, int y = 0) : x(x), y(y) {}
};

inline std::ostream& operator<<(std::ostream& os, const Position& pos) {
  os << "(" << pos.x << ", " << pos.y << ")";
  return os;
}

struct IslandPoint {
  Position position;
  int height;
  IslandPoint(Position position, int height = 10)
      : position(position), height(height) {}
};

inline std::ostream& operator<<(std::ostream& os,
                                const IslandPoint& height_map) {
  os << "(" << height_map.position << ", " << height_map.height << ")";
  return os;
}

// Globals

extern Logger log;

}  // namespace darena
