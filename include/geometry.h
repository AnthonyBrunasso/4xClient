#pragma once

#include <vector>
#include <GL/gl3w.h>

namespace geometry {
  std::vector<GLfloat>& get_triangle();
  std::vector<GLfloat>& get_triangle_color();
  
  std::vector<GLfloat> get_hexagon2d();
  void get_hexagon(std::vector<GLfloat>& vertices
    , std::vector<GLfloat>& normals
    , std::vector<GLuint>& indices);
}
