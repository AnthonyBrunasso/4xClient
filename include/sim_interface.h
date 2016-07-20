#pragma once

#include <unordered_map>
#include <vector>

#include "4xSimulation/include/tile.h"
#include "4xSimulation/include/world_map.h"
#include "4xSimulation/include/units.h"

namespace sim_interface {
  void initialize();
  void teardown();

  const world_map::TileMap& get_map();
  const std::vector<Unit>& get_units();

  void synch();

  // Poll for changes to simulation.
  bool poll();
}
