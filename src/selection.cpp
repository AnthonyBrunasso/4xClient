#include "selection.h"

#include "sim_interface.h"
#include "ui.h"

#include "client_util.h"

#include <iostream>

namespace selection {
  glm::ivec3 s_selected;
  Selection s_selection;

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

  void synch() {
    switch (s_selection.m_selection) {
      case SELECTION_TYPE::UNIT:
        synch_unit_selection();
      case SELECTION_TYPE::CITY:
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
  if (t == map.end() || !t->second.m_unit_ids.empty()) {
    const std::vector<Unit>& units = sim_interface::get_units();
    const Unit* u = util::id_binsearch(units.data(), units.size(), t->second.m_unit_ids.front());
    if (!u) return;
    s_selection.m_unit = *u;
    s_selection.m_selection = SELECTION_TYPE::UNIT;
    return;
  }

  // Clear the selection if a unit wasn't clicked on
  s_selection.clear();

  // Check if there is a city a the location to open the ui for it.
  if (t->second.m_city_id) ui::city(t->second.m_city_id);
}

// rclick should only act on the current selection
void selection::rclick(const glm::ivec3& location) {
  switch (s_selection.m_selection) {
    case SELECTION_TYPE::UNIT:
      sim_interface::move_unit(s_selection.m_unit.m_id, location);
      break;
    case SELECTION_TYPE::CITY:
    case SELECTION_TYPE::INACTIVE:
      break;
  }
}

const Selection& selection::get_selection() {
  return s_selection;
}
