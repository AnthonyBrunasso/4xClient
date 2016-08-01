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

  bool has_unit = false;
  const std::vector<Unit>& units = sim_interface::get_units();
  for (const auto& u : units) {
    if (u.m_location.x == location.x &&
        u.m_location.x == location.x &&
        u.m_location.x == location.x) {
      has_unit = true;
    }
  }

  if (has_unit) return;

  // Check if there is a city a the location to open the ui for it.
  const std::vector<City>& cities = sim_interface::get_cities();
  for (const auto& c : cities) {
    if (c.m_location.x == location.x &&
        c.m_location.x == location.x &&
        c.m_location.x == location.x) {
      ui::city(c.m_id);
    }
  }
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

