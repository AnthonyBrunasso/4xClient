#pragma once

#include <glm/vec3.hpp>

namespace selection {
  const glm::ivec3& get();
  void lclick(const glm::ivec3& location);
  void rclick(const glm::ivec3& location);
}
