#pragma once

#include <GL/gl3w.h>
#include <vector>
#include <string>

namespace shader {
  extern const char* BASIC_V;
  extern const char* BASIC_F;

  GLuint compile_from_file(GLenum type, const char* file); 
  GLuint compile_from_buffer(GLenum type, const char* buffer);

  GLuint link(std::vector<GLuint> shaders);
  std::string get_debug(GLuint program);

  bool valid(GLuint program);
}
