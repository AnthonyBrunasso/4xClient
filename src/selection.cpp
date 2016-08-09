#include "selection.h"

#include "sim_interface.h"
#include "ui.h"

#include "client_util.h"

#include <iostream>

namespace selection {
  glm::ivec3 s_selected;
  Selection s_selection;

  void rclick_unit(const glm::ivec3& loc);
  void rclick_harvest(const glm::ivec3& loc);
  void rclick_casting(const glm::ivec3& loc);

  void synch_unit_selection() {
    const std::vector<Unit>& units = sim_interface::get_units();
    const Unit* u = util::id_binsearch(units.data(), units.size(), s_selection.m_unit.m_id);
    // Remove the selected unit. It was likely killed during the last simulation step.
    if (!u) {
      s_selection.clear();
      return;
    }
    
    // Otherwise update the selection with the new unit state.
    s_selection.m_unit = *u;
  }

  void synch_city_selection() {
    const std::vector<City>& cities = sim_interface::get_cities();
    const City* c = util::id_binsearch(cities.data(), cities.size(), s_selection.m_city.m_id);
    if (!c) {
      s_selection.clear();
      return;
    }
    s_selection.m_city = *c;
  }

  void synch() {
    switch (s_selection.m_selection) {
      case SELECTION_TYPE::UNIT:
        synch_unit_selection();
        break;
      case SELECTION_TYPE::CITY:
        synch_city_selection();
        break;
      case SELECTION_TYPE::INACTIVE:
        break;
    }
  }
}

void selection::initialize() {
  sim_interface::sub_synch(synch);
}

const glm::ivec3& selection::get() {
  return s_selected;
}

void selection::lclick(const glm::ivec3& location) {
  s_selected = location;
  sf::Vector3i loc(location.x, location.y, location.z);

  const world_map::TileMap& map = sim_interface::get_map();
  const auto& t = map.find(loc);

  if (t == map.end()) return;

  if (!t->second.m_unit_ids.empty()) {
    const std::vector<Unit>& units = sim_interface::get_units();
    const Unit* u = util::id_binsearch(units.data(), units.size(), t->second.m_unit_ids.front());
    if (!u) return;
    // Close city ui if it's open.
    ui::city(0);
    s_selection.m_unit = *u;
    s_selection.m_selection = SELECTION_TYPE::UNIT;
    return;
  }

  // Clear the selection if a unit wasn't clicked on
  s_selection.clear();
  ui::city(0);

  // Check if there is a city a the location to open the ui for it.
  if (t->second.m_city_id) {
    const std::vector<City>& cities = sim_interface::get_cities();
    const City* c = util::id_binsearch(cities.data(), cities.size(), t->second.m_city_id);
    if (!c) return;
    s_selection.m_city = *c;
    s_selection.m_selection = SELECTION_TYPE::CITY;
    ui::city(t->second.m_city_id);
    return;
  }
}

void selection::rclick_unit(const glm::ivec3& loc) {
  // Check if the target has an enemy on it.
  const world_map::TileMap& map = sim_interface::get_map();
  const std::vector<Unit>& units = sim_interface::get_units();

  sf::Vector3i sloc(loc.x, loc.y, loc.z);
  const auto& t = map.find(sloc);
  if (t == map.end()) return;

  // Prioritize hitting a city.
  if (t->second.m_city_id) {
    // Get the current player, make sure they don't own the city.
    const Player* player = sim_interface::get_currentplayerptr();
    if (!player->OwnsCity(t->second.m_city_id)) {
      sim_interface::siege(s_selection.m_unit.m_id, t->second.m_city_id);
      return;
    }
  }

  // Otherwise try to attack a unit.
  if (!t->second.m_unit_ids.empty()) {
    // Loop through and see if an enemy unit can be found.
    for (const auto& u : t->second.m_unit_ids) {
      const Unit* target = util::id_binsearch(units.data(), units.size(), u);
      if (!target) continue;
      if (target->m_owner_id != s_selection.m_unit.m_owner_id) {
        // Try to attack the unit, simulation will fail if the unit is out of range.
        sim_interface::attack(s_selection.m_unit.m_id, u);
        return;
      }
    }
  }

  // Finally, move to a tile if a unit or city can't be attacked.
  sim_interface::move_unit(s_selection.m_unit.m_id, loc);
}

void selection::rclick_harvest(const glm::ivec3& loc) {
  const world_map::TileMap& map = sim_interface::get_map();
  const std::vector<City>& cities = sim_interface::get_cities();

  sf::Vector3i sloc(loc.x, loc.y, loc.z);
  const auto& t = map.find(sloc);
  if (t == map.end()) return;

  sim_interface::harvest(loc);
}

void selection::rclick_casting(const glm::ivec3& loc) {
  const world_map::TileMap& map = sim_interface::get_map();
  sf::Vector3i sloc(loc.x, loc.y, loc.z);
  const auto& t = map.find(sloc);
  if (t == map.end()) return;
  
  sim_interface::cast(loc, s_selection.m_magic);
}

// rclick should only act on the current selection
void selection::rclick(const glm::ivec3& location) {
  switch (s_selection.m_selection) {
    case SELECTION_TYPE::UNIT:
      rclick_unit(location);
      break;
    case SELECTION_TYPE::CITY:
      break;
    case SELECTION_TYPE::HARVEST:
      rclick_harvest(location);
      break;
    case SELECTION_TYPE::CASTING:
      rclick_casting(location);
      break;
    case SELECTION_TYPE::INACTIVE:
      break;
  }
}

Selection& selection::get_selection() {
  return s_selection;
}

void selection::set_selection(SELECTION_TYPE type) {
  s_selection.m_selection = type;
}
