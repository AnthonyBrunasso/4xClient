#pragma once

#include <glm/vec3.hpp>
#include <vector>

namespace e4x {

class Mesh;

namespace light {
  void add(const glm::vec3& pos);
  const std::vector<glm::vec3>& get();
  void apply(Mesh* m);
}

}