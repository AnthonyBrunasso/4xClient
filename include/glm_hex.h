#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstdint>
#include <vector>

// glm namespace to avoid collision with simulations hex library
namespace glm_hex
{
  // Hexagons coordinates have 6 neighbors
  const static unsigned int NEIGHBOR_COUNT = 6;

  // Given a hexagon, the directos are:
  //        
  //  North West   / \   North East
  //  West        |   |  East
  //  South West   \ /   South East
  // 

  // Cube coordinates for hex grid must maintain equality: x + y + z = 0
  const static glm::ivec3 CUBE_DIRECTIONS[NEIGHBOR_COUNT] = {
    glm::ivec3(1, -1, 0), // North East
    glm::ivec3(1, 0, -1), // East
    glm::ivec3(0, 1, -1), // South East
    glm::ivec3(-1, 1, 0), // South West
    glm::ivec3(-1, 0, 1), // West
    glm::ivec3(0, -1, 1), // North West
  };

  // Axial coordinates simply drop the third coordinate of cube
  const static glm::ivec2 AXIAL_DIRECTIONS[NEIGHBOR_COUNT] = {
    glm::ivec2(1 ,  0), // East
    glm::ivec2(1 , -1), // North East
    glm::ivec2(0 , -1), // North West
    glm::ivec2(-1,  0), // West
    glm::ivec2(-1,  1), // South West
    glm::ivec2(0 ,  1), // South East
  };

  struct CubeNeighbors {
    CubeNeighbors(const glm::ivec3& start);
    glm::ivec3& operator[](uint32_t i) { return neighbors[i]; }
    glm::ivec3 neighbors[NEIGHBOR_COUNT];
  };

  struct AxialNeighbors {
    AxialNeighbors(const glm::ivec2& start);
    glm::ivec2& operator[](uint32_t i) { return neighbors[i]; }
    glm::ivec2 neighbors[NEIGHBOR_COUNT];
  };

  void cube_neighbors(const glm::ivec3& start, std::vector<glm::ivec3>& neighbors);

  // Grid conversions
  glm::ivec2 cube_to_axial(const glm::ivec3& cube_coord);
  glm::vec2 cube_to_axial(const glm::vec3& cube_coord);

  glm::ivec2 cube_to_offset(const glm::ivec3& cube_coord);
  glm::ivec2 axial_to_offset(const glm::ivec2& axial_coord);

  glm::ivec3 axial_to_cube(const glm::ivec2& axial_coord);
  glm::vec3 axial_to_cube(const glm::vec2& axial_coord);

  // Get the pixel coordinates to the i'th corner of a hexagon offset from (0,0)
  glm::vec2 hex_corner(uint32_t size, uint32_t i);

  // Hex and pixel conversions
  glm::vec2 axial_to_world(const glm::ivec2& axial_coord, uint32_t size);
  glm::vec2 cube_to_world(const glm::ivec3& cube_coord, uint32_t size);
  glm::vec2 offset_to_world(const glm::ivec2& axial_coord, uint32_t size);

  glm::ivec2 world_to_axial(const glm::vec2& world_coord, uint32_t size);
  glm::ivec3 world_to_cube(const glm::vec2& world_coord, uint32_t size);
  glm::ivec2 world_to_offset(const glm::vec2& world_coord, uint32_t size);

  // Rounding pixel coord to axial and cube coordinates
  // Cube coordinates must mantain x + y + z = 0 therefore a round of componenets isn't enough
  glm::ivec3 cube_round(const glm::vec3& pixel_coord);
  glm::ivec2 axial_round(const glm::vec2& pixel_coord);

  // Distance of coordinates
  uint32_t cube_distance(const glm::ivec3& a, const glm::ivec3& b);
  uint32_t axial_distance(const glm::ivec2& a, const glm::ivec2& b);

  // This function requires floating point cube-coordinates else a lerp on integer
  // vectors would likely not maintain the equality x + y + z = 0
  void cubes_on_line(const glm::vec3& a, const glm::vec3& b, std::vector<glm::ivec3>& coords);
}
