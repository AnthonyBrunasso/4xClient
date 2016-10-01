#pragma once

namespace e4x {
  class Mesh;
}

#include <glm/vec3.hpp>

namespace map {
  void initialize();
  void teardown();
  void update(double delta);
  void draw();

  e4x::Mesh* get_tile_mesh();

  const glm::ivec3& get_hover();
}
