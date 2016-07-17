#pragma once

#include <vector>
#include <GL/gl3w.h>

namespace geometry {
  std::vector<GLfloat>& get_triangle();
  void get_triangle(std::vector<GLfloat>& vertices
    , std::vector<GLfloat>& normals);

  std::vector<GLfloat>& get_square();
  std::vector<GLfloat>& get_squaretexcoords();
  void get_square(std::vector<GLfloat>& vertices
    , std::vector<GLfloat>& normals);

  std::vector<GLfloat> get_hexagon2d();
  void get_hexagon(std::vector<GLfloat>& vertices
    , std::vector<GLfloat>& normals
    , std::vector<GLuint>& indices);
}
