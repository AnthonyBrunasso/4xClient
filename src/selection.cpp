#include "selection.h"

#include "sim_interface.h"
#include "ui.h"

#include <iostream>

namespace selection {
  glm::ivec3 s_selected;
}

const glm::ivec3& selection::get() {
  return s_selected;
}

void selection::lclick(const glm::ivec3& location) {
  s_selected = location;
  sf::Vector3i loc(location.x, location.y, location.z);

  const world_map::TileMap& map = sim_interface::get_map();
  const auto& t = map.find(loc);
  if (t == map.end() || !t->second.m_unit_ids.empty()) return;
  // Check if there is a city a the location to open the ui for it.
  if (t->second.m_city_id) ui::city(t->second.m_city_id);
}

void selection::rclick(const glm::ivec3& location) {
  // Check if s_selected has a unit at its location.
  const world_map::TileMap& tiles = sim_interface::get_map();
  sf::Vector3i sfvec(s_selected.x, s_selected.y, s_selected.z);
  const auto& t = tiles.find(sfvec);
  if (t == tiles.end()) return;
  if (t->second.m_unit_ids.empty()) return;
  // Otherwise try to move the first unit in the list to the selected location.
  sim_interface::move_unit(t->second.m_unit_ids.at(0), location);
  s_selected = location;
}

