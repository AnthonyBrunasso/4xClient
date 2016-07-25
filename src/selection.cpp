#include "selection.h"

#include "sim_interface.h"

#include <iostream>

namespace selection {
  glm::ivec3 s_selected;
}

const glm::ivec3& selection::get() {
  return s_selected;
}

void selection::lclick(const glm::ivec3& location) {
  s_selected = location;
}

void selection::rclick(const glm::ivec3& location) {
  // Check if s_selected has a unit at its location.
  const world_map::TileMap& tiles = sim_interface::get_map();
  sf::Vector3i sfvec(s_selected.x, s_selected.y, s_selected.z);
  const auto& t = tiles.find(sfvec);
  if (t == tiles.end()) {
    return;
  }

  if (t->second.m_unit_ids.empty()) return;
  // Otherwise try to move the first unit in the list to the selected location.
  sim_interface::move_unit(t->second.m_unit_ids.at(0), location);
  s_selected = location;
}

