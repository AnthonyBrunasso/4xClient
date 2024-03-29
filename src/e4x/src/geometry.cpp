#include "geometry.h"

#include "glm_hex.h"

#include <cmath>

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
    glm::vec2 corner = glm_hex::hex_corner(3, i);
    hexagon.push_back(corner.x);
    hexagon.push_back(corner.y);
    hexagon.push_back(0.0);

    corner = glm_hex::hex_corner(3, (i + 1) % 6);
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
  float depth = 0.0f;
  vertices.clear();

  for (uint32_t i = 0; i < 6; ++i) {
    // All triangles in hexagon have a vertex at 0,0,0
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(depth);

    normals.push_back(0.0f);
    normals.push_back(0.0f);
    normals.push_back(1.0f);

    // Get hex corner.
    glm::vec2 corner = glm_hex::hex_corner(3, i);
    vertices.push_back(corner.x);
    vertices.push_back(corner.y);
    vertices.push_back(depth);

    normals.push_back(0.0f);
    normals.push_back(0.0f);
    normals.push_back(1.0f);

    corner = glm_hex::hex_corner(3, (i + 1) % 6);
    vertices.push_back(corner.x);
    vertices.push_back(corner.y);
    vertices.push_back(depth);

    normals.push_back(0.0f);
    normals.push_back(0.0f);
    normals.push_back(1.0f);
  }
}

std::vector<GLfloat> geometry::get_hexagontexcoords() {
  std::vector<GLfloat> texcoords;
  // 30 degrees.
  GLfloat x = 0.5f * tan(30.0f * 3.14159265f / 180.0f);
  GLfloat d = 1.0f - x;

  // Tri 1
  texcoords.push_back(0.5f);
  texcoords.push_back(0.5f);

  texcoords.push_back(0.5f);
  texcoords.push_back(0.0f);

  texcoords.push_back(1.0f);
  texcoords.push_back(x);

  // Tri 2
  texcoords.push_back(0.5f);
  texcoords.push_back(0.5f);

  texcoords.push_back(1.0f);
  texcoords.push_back(x);

  texcoords.push_back(1.0f);
  texcoords.push_back(d);

  // Tri 3
  texcoords.push_back(0.5f);
  texcoords.push_back(0.5f);

  texcoords.push_back(1.0f);
  texcoords.push_back(d);

  texcoords.push_back(0.5f);
  texcoords.push_back(1.0f);

  // Tri 4
  texcoords.push_back(0.5f);
  texcoords.push_back(0.5f);

  texcoords.push_back(0.5f);
  texcoords.push_back(1.0f);

  texcoords.push_back(0.0f);
  texcoords.push_back(d);

  // Tri 5
  texcoords.push_back(0.5f);
  texcoords.push_back(0.5f);

  texcoords.push_back(0.0f);
  texcoords.push_back(d);

  texcoords.push_back(0.0f);
  texcoords.push_back(x);

  // Tri 6
  texcoords.push_back(0.5f);
  texcoords.push_back(0.5f);

  texcoords.push_back(0.0f);
  texcoords.push_back(x);

  texcoords.push_back(0.5f);
  texcoords.push_back(0.0f);

  return texcoords;
}

std::vector<GLfloat> geometry::get_hexagontangents() {
  std::vector<GLfloat> tangents;
  for (uint32_t i = 0; i < 18; ++i) {
    tangents.push_back(1.0f);
    tangents.push_back(0.0f);
    tangents.push_back(0.0f);
  }
  return tangents;
}
