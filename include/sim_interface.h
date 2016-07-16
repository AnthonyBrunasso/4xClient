#pragma once

#include <unordered_map>

#include "4xSimulation/include/tile.h"
#include "4xSimulation/include/world_map.h"

namespace sim_interface {
  void start();
  void teardown();

  world_map::TileMap get_map();
}
