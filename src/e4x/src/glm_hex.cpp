#include "glm_hex.h"

#include <glm/gtx/compatibility.hpp>

#include <algorithm> 
#include <cstdlib>
#include <cmath>

namespace {
  const static float PI = 3.1415926535897f;
  const static float SQRT_3 = 1.73205080757f;
}

glm_hex::CubeNeighbors::CubeNeighbors(const glm::ivec3& start) {
  for (uint32_t i = 0; i < NEIGHBOR_COUNT; ++i) {
    neighbors[i] = start + CUBE_DIRECTIONS[i];
  }
}

glm_hex::AxialNeighbors::AxialNeighbors(const glm::ivec2& start) {
  for (uint32_t i = 0; i < NEIGHBOR_COUNT; ++i) {
    neighbors[i] = start + AXIAL_DIRECTIONS[i];
  }
}
  
void glm_hex::cube_neighbors(const glm::ivec3& start, std::vector<glm::ivec3>& neighbors) {
  neighbors.clear();
  for (uint32_t i = 0; i < NEIGHBOR_COUNT; ++i) {
    neighbors.push_back(start + CUBE_DIRECTIONS[i]);
  }
}

glm::ivec2 glm_hex::cube_to_axial(const glm::ivec3& cube_coord) {
  return glm::ivec2(cube_coord.x, cube_coord.y);
}

glm::vec2 glm_hex::cube_to_axial(const glm::vec3& cube_coord) {
  return glm::vec2(cube_coord.x, cube_coord.y);
}

glm::ivec2 glm_hex::cube_to_offset(const glm::ivec3& cube_coord) {
  int32_t col = cube_coord.x + (cube_coord.z - (cube_coord.z & 1)) / 2;
  int32_t row = cube_coord.z;
  return glm::ivec2(col, row);
}

glm::ivec2 glm_hex::axial_to_offset(const glm::ivec2& axial_coord) {
  return cube_to_offset(axial_to_cube(axial_coord));
}

glm::ivec3 glm_hex::axial_to_cube(const glm::ivec2& axial_coord) {
  return glm::ivec3(axial_coord.x, axial_coord.y, -axial_coord.x - axial_coord.y);
}

glm::vec3 glm_hex::axial_to_cube(const glm::vec2& axial_coord) {
  return glm::vec3(axial_coord.x, axial_coord.y, -axial_coord.x - axial_coord.y);
}

glm::vec2 glm_hex::hex_corner(uint32_t size, uint32_t i) {
  // Corners of hex are 60 degrees apart starting at 30 degress (lower right corner)
  const uint32_t angle = 60 * i + 30;
  const float rad = PI / 180.0f * angle;
  return glm::vec2(size * cos(rad), size * sin(rad));
}

glm::vec2 glm_hex::axial_to_world(const glm::ivec2& axial_coord, uint32_t size) {
  const float x = size * SQRT_3 * (axial_coord.x + (axial_coord.y / 2.0f));
  const float y = size * (3.0f / 2.0f) * axial_coord.y;
  return glm::vec2(x, y);
}

glm::vec2 glm_hex::cube_to_world(const glm::ivec3& cube_coord, uint32_t size) {
  return axial_to_world(cube_to_axial(cube_coord), size);
}

glm::vec2 glm_hex::offset_to_world(const glm::ivec2& axial_coord, uint32_t size) {
  // Bitwise & is to tell if the cord is odd or not. We offset every odd row
  const float x = size * SQRT_3 * (axial_coord.x + 0.5f * (axial_coord.y & 1));
  const float y = size * (3.0f / 2.0f) * axial_coord.y;
  return glm::vec2(x, y);
}

glm::ivec2 glm_hex::world_to_axial(const glm::vec2& world_coord, uint32_t size) {
  const float x = (world_coord.x * (SQRT_3 / 3.0f) - world_coord.y / 3.0f) / size;
  const float y = world_coord.y * (2.0f / 3.0f) / size;
  return axial_round(glm::vec2(x, y));
}

glm::ivec3 glm_hex::world_to_cube(const glm::vec2& world_coord, uint32_t size) {
  return axial_to_cube(world_to_axial(world_coord, size));
}

glm::ivec2 glm_hex::world_to_offset(const glm::vec2& world_coord, uint32_t size) {
  return axial_to_offset(world_to_axial(world_coord, size));
}

// Rounding must maintain x + y + z = 0 
glm::ivec3 glm_hex::cube_round(const glm::vec3& cube_coord) {
  // Round all the components
  int32_t rx = static_cast<int32_t>(roundf(cube_coord.x));
  int32_t ry = static_cast<int32_t>(roundf(cube_coord.y));
  int32_t rz = static_cast<int32_t>(roundf(cube_coord.z));

  // Fix them to maintain equality
  const float dx = fabs(rx - cube_coord.x);
  const float dy = fabs(ry - cube_coord.y);
  const float dz = fabs(rz - cube_coord.z);

  if (dx > dy && dx > dz) {
    rx = -ry - rz;
  }
  else if (dy > dz) {
    ry = -rx - rz;
  }
  else {
    rz = -rx - ry;
  }

  return glm::ivec3(rx, ry, rz);
}

glm::ivec2 glm_hex::axial_round(const glm::vec2& axial_coord) {
  return cube_to_axial(cube_round(axial_to_cube(axial_coord)));
}

uint32_t glm_hex::cube_distance(const glm::ivec3& a, const glm::ivec3& b) {
  return std::max( { std::abs(a.x - b.x), std::abs(a.y - b.y), std::abs(a.z - b.z) } );
}

uint32_t glm_hex::axial_distance(const glm::ivec2& a, const glm::ivec2& b) {
  // Convert to cube coordinates and get distance
  glm::ivec3 ac = axial_to_cube(a);
  glm::ivec3 bc = axial_to_cube(b);
  return cube_distance(ac, bc);
}

void glm_hex::cubes_on_line(const glm::vec3& a, const glm::vec3& b, std::vector<glm::ivec3>& coords) {
  uint32_t distance = cube_distance(cube_round(a), cube_round(b));
  for (uint32_t i = 0; i <= distance; ++i) {
    coords.push_back(cube_round(glm::lerp(a, b, 1.0f / distance * i)));
  }
}
