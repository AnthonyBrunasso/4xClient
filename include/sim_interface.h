#pragma once

#include <unordered_map>
#include <vector>

#include "tile.h"
#include "world_map.h"
#include "unit.h"
#include "city.h"
#include "player.h"
#include "notification.h"

#include <glm/vec3.hpp>
#include <functional>

enum class MULTIPLAYER {
  NO = 0,
  YES = 1,
};

namespace sim_interface {

  void initialize(MULTIPLAYER multiplayer);
  void teardown();

  void attack(uint32_t from_id, uint32_t to_id);
  void siege(uint32_t unit_id, uint32_t city_id);
  void cast(const glm::ivec3& location, MAGIC_TYPE type);
  void harvest(const glm::ivec3& loc);
  void move_unit(uint32_t id, const glm::ivec3& location);
  void construct(uint32_t city_id, CONSTRUCTION_TYPE type);
  void specialize(uint32_t city_id, TERRAIN_TYPE type);
  void end_turn();

  void join_player();
  // Take the active player as your own
  // Useful for resuming a game or shared control scenarios
  void resume_player();
  void join_barbarian();
  bool is_me(uint32_t player);
  void initial_settle();
  void settle();
  void production_abort(uint32_t city, uint32_t index);
  void production_move(uint32_t city, uint32_t source, uint32_t dest);

  const world_map::TileMap& get_map();
  const std::vector<Unit>& get_units();
  const std::vector<City>& get_cities();
  const std::vector<Player>& get_players();
  const NotificationVector& get_player_notifications(uint32_t player_id);

  uint32_t get_currentplayer();
  const Player* get_currentplayerptr();

  void synch();
  void sub_synch(std::function<void()> sub);

  // Poll for changes to simulation.
  bool poll();
}
