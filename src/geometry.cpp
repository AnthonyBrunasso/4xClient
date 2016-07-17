#include "geometry.h"

#include "glm_hex.h"

void geometry::get_triangle(std::vector<GLfloat>& vertices
    , std::vector<GLfloat>& normals) {
  vertices = geometry::get_triangle();
  normals = {
    0.0f, 0.0f, 1.0f
  };
}

std::vector<GLfloat>& geometry::get_triangle() {
  static std::vector<GLfloat> triangle = {
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
  };
  return triangle;
}

std::vector<GLfloat>& geometry::get_square() {
  static std::vector<GLfloat> square = {
    -0.5f, -0.5f,  0.0f,
    0.5f, -0.5f,  0.0f,
    0.5f,  0.5f,  0.0f,
    0.5f,  0.5f,  0.0f,
    -0.5f,  0.5f,  0.0f,
    -0.5f, -0.5f,  0.0f
  };
  return square;
}
void geometry::get_square(std::vector<GLfloat>& vertices
    , std::vector<GLfloat>& normals) {
  vertices = geometry::get_square();
  normals = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
  };
}

std::vector<GLfloat>& geometry::get_squaretexcoords() {
  static std::vector<GLfloat> square = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f
  };
  return square;
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

void geometry::get_hexagon(std::vector<GLfloat>& vertices
  , std::vector<GLfloat>& normals
  , std::vector<GLuint>& indices) {
  std::vector<GLfloat> hexagon;
  float depth = 0.3f;
  vertices.clear();

  for (uint32_t i = 0; i < 6; ++i) {
    // All triangles in hexagon have a vertex at 0,0,0
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    normals.push_back(0.0f);
    normals.push_back(0.0f);
    normals.push_back(-1.0f);

    // Get hex corner.
    glm::vec2 corner1 = glm_hex::hex_corner(1, i);
    vertices.push_back(corner1.x);
    vertices.push_back(corner1.y);
    vertices.push_back(0.0);

    normals.push_back(0.0f);
    normals.push_back(0.0f);
    normals.push_back(-1.0f);

    glm::vec2 corner2 = glm_hex::hex_corner(1, (i + 1) % 6);
    vertices.push_back(corner2.x);
    vertices.push_back(corner2.y);
    vertices.push_back(0.0f);

    normals.push_back(0.0f);
    normals.push_back(0.0f);
    normals.push_back(-1.0f);
  }

  for (uint32_t i = 0; i < 6; ++i) {
    // All triangles in hexagon have a vertex at 0,0,0
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(depth);

    normals.push_back(0.0f);
    normals.push_back(0.0f);
    normals.push_back(1.0f);

    // Get hex corner.
    glm::vec2 corner = glm_hex::hex_corner(1, i);
    vertices.push_back(corner.x);
    vertices.push_back(corner.y);
    vertices.push_back(depth);

    normals.push_back(0.0f);
    normals.push_back(0.0f);
    normals.push_back(1.0f);

    corner = glm_hex::hex_corner(1, (i + 1) % 6);
    vertices.push_back(corner.x);
    vertices.push_back(corner.y);
    vertices.push_back(depth);

    normals.push_back(0.0f);
    normals.push_back(0.0f);
    normals.push_back(1.0f);
  }
}
