#include "geometry.h"

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
