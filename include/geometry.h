#pragma once

#include <vector>
#include <gl/gl3w.h>

namespace geometry {
  std::vector<GLfloat>& get_triangle();
  std::vector<GLfloat>& get_triangle_color();
  
  std::vector<GLfloat> get_hexagon2d();
}
