#include "island.h"

#include <SDL_opengl.h>

#include <earcut.hpp>
#include <string>

#include "game.h"

namespace darena {

std::string Island::to_string() const {
  std::string output = "Island(";
  for (auto it = heightmap.begin(); it != heightmap.end(); it++) {
    output.append(it->to_string());
  }
  output.append(")");
  return output;
}

void Island::build_island_part(size_t start_i, size_t end_i) {
  std::vector<darena::IslandPoint> inside = {};

  // Top vertices left to right
  for (size_t i = start_i; i <= end_i; ++i) {
    inside.push_back(heightmap.at(i));
  }

  // Bottom vertices right to left, needed for triangulation
  for (size_t i = 0; i <= (end_i - start_i); ++i) {
    size_t current_map_i = end_i - i;
    inside.emplace_back(darena::Vec2{heightmap[current_map_i].position.x,
                                     (float)(ISLAND_Y_OFFSET + ISLAND_HEIGHT)},
                        heightmap[current_map_i].strength);
  }

  island_vertices.push_back(inside);
}

void Island::rebuild_island_mesh() {
  island_vertices.clear();
  island_indices.clear();

  // If the island is split into segments, we need to build multiple polygons
  size_t start_i = 0;
  bool last_was_zero = false;
  for (size_t i = 0; i < heightmap.size(); ++i) {
    bool zero = are_equal(heightmap[i].position.y,
                          (float)(ISLAND_Y_OFFSET + ISLAND_HEIGHT));
    if (!zero && !last_was_zero) {
      start_i = i;
      last_was_zero = true;
    } else if (zero && last_was_zero) {
      if ((i - 1) > start_i) {
        build_island_part(start_i, i - 1);
      }
      last_was_zero = false;
    }
  }

  if (last_was_zero) {
    size_t last_heightmap_i = heightmap.size() - 1;
    if (last_heightmap_i > start_i) {
      build_island_part(start_i, last_heightmap_i);
    }
  }

  // Triangulation step
  for (const auto& island_part : island_vertices) {
    if (island_part.size() < 3) {
      // Shouldn't happen as build_island_part should always produce >= 4
      // points. Earcut requires at least 3 vertices to form a polygon.

      // Add an empty list of indices
      island_indices.emplace_back();
      darena::log << "Vertex generation error for island!\n";
      continue;
    }

    // Earcut expects input as a vector of rings, where each ring is a vector of
    // 2D points.
    std::vector<std::vector<std::array<float, 2>>> earcut_input_polygon;
    std::vector<std::array<float, 2>> part_of_polygon;
    part_of_polygon.reserve(island_part.size());

    for (const auto& island_point : island_part) {
      part_of_polygon.push_back(
          {island_point.position.x, island_point.position.y});
    }

    earcut_input_polygon.push_back(part_of_polygon);

    std::vector<uint> resulting_indices =
        mapbox::earcut<uint>(earcut_input_polygon);
    island_indices.push_back(resulting_indices);
  }
}

void Island::process_input(Game* game, SDL_Event* e) { return; }

void Island::update(Game* game, float delta_time) { return; }

void Island::render(Game* game) {
  glColor3f(1.0f, 1.0f, 1.0f);
  for (size_t i = 0; i < island_indices.size(); ++i) {
    const std::vector<uint>& indices = island_indices[i];
    const std::vector<darena::IslandPoint>& vertices = island_vertices[i];

    glBegin(GL_TRIANGLES);
    for (size_t j = 0; j < indices.size(); ++j) {
      uint vertex_index = indices[j];
      if (vertex_index < vertices.size()) {
        const darena::IslandPoint& point = vertices[vertex_index];
        glVertex2f(point.position.x, point.position.y);
      } else {
        darena::log << "Vertex index out of bounds!\n";
      }
    }
    glEnd();
  }
}

void Island::deprecated_gl_island_render(darena::Game* game) {
  glColor3f(1.0f, 1.0f, 1.0f);

  glBegin(GL_POLYGON);
  // Bottom left point first
  glVertex2i(heightmap[0].position.x, ISLAND_Y_OFFSET + ISLAND_HEIGHT);

  bool last_was_zero = false;
  for (size_t i = 0; i < heightmap.size(); ++i) {
    const darena::IslandPoint& point = heightmap[i];
    if (!are_equal(point.position.y,
                   (float)(ISLAND_Y_OFFSET + ISLAND_HEIGHT))) {
      if (last_was_zero) {
        glVertex2i(point.position.x, ISLAND_Y_OFFSET + ISLAND_HEIGHT);
      }
      glVertex2i(point.position.x, point.position.y);
      last_was_zero = false;
      continue;
    }
    if (!last_was_zero && i - 1 > 0) {
      glVertex2i(heightmap[i - 1].position.x, ISLAND_Y_OFFSET + ISLAND_HEIGHT);
    }
    glEnd();
    glBegin(GL_POLYGON);
    last_was_zero = true;
  }

  // Bottom right point last
  glVertex2i(heightmap.back().position.x, ISLAND_Y_OFFSET + ISLAND_HEIGHT);
  glEnd();
}

}  // namespace darena
