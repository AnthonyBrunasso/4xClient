#pragma once

#include <unordered_map>
#include <vector>

#include "4xSimulation/include/tile.h"
#include "4xSimulation/include/world_map.h"
#include "4xSimulation/include/units.h"

#include <glm/vec3.hpp>

namespace sim_interface {
  void initialize();
  void teardown();

  void move_unit(uint32_t id, const glm::ivec3& location);

  const world_map::TileMap& get_map();
  const std::vector<Unit>& get_units();

  void synch();

  // Poll for changes to simulation.
  bool poll();
}
