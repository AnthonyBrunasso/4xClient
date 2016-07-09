#include "geometry.h"

#include "glm_hex.h"

std::vector<GLfloat>& geometry::get_triangle() {
  static std::vector<GLfloat> triangle = {
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
  };
  return triangle;
}

std::vector<GLfloat>& geometry::get_triangle_color() {
  static std::vector<GLfloat> triangle_color = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
  };
  return triangle_color;
}

std::vector<GLfloat> geometry::get_hexagon2d() {
  std::vector<GLfloat> hexagon;

  for (uint32_t i = 0; i < 6; ++i) {
    // All triangles in hexagon have a vertex at 0,0,0
    hexagon.push_back(0.0f);
    hexagon.push_back(0.0f);
    hexagon.push_back(0.0f);

    // Get hex corner.
    glm::vec2 corner = glm_hex::hex_corner(1, i);
    hexagon.push_back(corner.x);
    hexagon.push_back(corner.y);
    hexagon.push_back(0.0);

    corner = glm_hex::hex_corner(1, (i + 1) % 6);
    hexagon.push_back(corner.x);
    hexagon.push_back(corner.y);
    hexagon.push_back(0.0);
  }

  return hexagon;
}

std::vector<GLfloat> geometry::get_hexagon() {
  std::vector<GLfloat> hexagon;
  float depth = 0.3f;

  for (uint32_t i = 0; i < 6; ++i) {
    // All triangles in hexagon have a vertex at 0,0,0
    hexagon.push_back(0.0f);
    hexagon.push_back(0.0f);
    hexagon.push_back(0.0f);

    // Get hex corner.
    glm::vec2 corner1 = glm_hex::hex_corner(1, i);
    hexagon.push_back(corner1.x);
    hexagon.push_back(corner1.y);
    hexagon.push_back(0.0);

    glm::vec2 corner2 = glm_hex::hex_corner(1, (i + 1) % 6);
    hexagon.push_back(corner2.x);
    hexagon.push_back(corner2.y);
    hexagon.push_back(0.0);

    // Create the triangles for the width.
    hexagon.push_back(corner1.x);
    hexagon.push_back(corner1.y);
    hexagon.push_back(0.0f);

    hexagon.push_back(corner1.x);
    hexagon.push_back(corner1.y);
    hexagon.push_back(depth);

    hexagon.push_back(corner2.x);
    hexagon.push_back(corner2.y);
    hexagon.push_back(0.0);

    hexagon.push_back(corner1.x);
    hexagon.push_back(corner1.y);
    hexagon.push_back(depth);

    hexagon.push_back(corner2.x);
    hexagon.push_back(corner2.y);
    hexagon.push_back(depth);

    hexagon.push_back(corner2.x);
    hexagon.push_back(corner2.y);
    hexagon.push_back(0.0);
  }

  for (uint32_t i = 0; i < 6; ++i) {
    // All triangles in hexagon have a vertex at 0,0,0
    hexagon.push_back(0.0f);
    hexagon.push_back(0.0f);
    hexagon.push_back(depth);

    // Get hex corner.
    glm::vec2 corner = glm_hex::hex_corner(1, i);
    hexagon.push_back(corner.x);
    hexagon.push_back(corner.y);
    hexagon.push_back(depth);

    corner = glm_hex::hex_corner(1, (i + 1) % 6);
    hexagon.push_back(corner.x);
    hexagon.push_back(corner.y);
    hexagon.push_back(depth);
  }

  return hexagon;
}
