#pragma once

#include <GL/gl3w.h>

namespace texloader {
  unsigned char* read(const char* filename, int& width, int& height);
  void load(const char* filename, GLenum slot, GLuint& tex);
}
