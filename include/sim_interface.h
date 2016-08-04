#pragma once

#include <unordered_map>
#include <vector>

#include "4xSimulation/include/tile.h"
#include "4xSimulation/include/world_map.h"
#include "4xSimulation/include/unit.h"
#include "4xSimulation/include/city.h"
#include "4xSimulation/include/player.h"

#include <glm/vec3.hpp>

enum class MULTIPLAYER {
  NO = 0,
  YES = 1,
};

namespace sim_interface {

  void initialize(MULTIPLAYER multiplayer);
  void teardown();

  void move_unit(uint32_t id, const glm::ivec3& location);
  void construct(uint32_t city_id, CONSTRUCTION_TYPE type);
  void end_turn();

  void join_player();
  void join_barbarian();
  void initial_settle();
  void settle();

  const world_map::TileMap& get_map();
  const std::vector<Unit>& get_units();
  const std::vector<City>& get_cities();
  const std::vector<Player>& get_players();

  void synch();

  // Poll for changes to simulation.
  bool poll();
}
