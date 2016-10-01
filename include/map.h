#pragma once

#include "e4x/include/glm/vec3.hpp"
#include "e4x/include/mesh.h"

namespace map {
  void initialize();
  void teardown();
  void update(double delta);
  void draw();

  e4x::Mesh* get_tile_mesh();

  const glm::ivec3& get_hover();
}
