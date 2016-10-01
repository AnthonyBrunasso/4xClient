#pragma once

// A cache for creating a storing gl programs.

#include <GL/gl3w.h>

#include <vector>
#include <string>
#include <functional>

namespace program {
  void build(const std::string& name,
      const std::vector<std::pair<GLenum, std::string> >& shaders);
  void remove(const std::string& name);

  GLuint get(const std::string& name);
  // Force a rebuild on a programj
  void rebuid(const std::string& name);
  void sub_rebuild(std::function<void(GLuint, GLuint)> sub);
}
