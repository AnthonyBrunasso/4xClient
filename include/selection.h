#pragma once

#include <glm/vec3.hpp>

#include "player.h"
#include "city.h"
#include "unit.h"

enum SELECTION_TYPE {
  UNIT,
  CITY,
  INACTIVE,
};

struct Selection {
  Selection() : m_player(0, "", 0, AI_TYPE::UNKNOWN), m_unit(0, UNIT_TYPE::UNKNOWN), m_city(0), m_selection(SELECTION_TYPE::INACTIVE) {};
  Player m_player;
  Unit m_unit;
  City m_city;
  SELECTION_TYPE m_selection;

  void clear() {
    m_player.m_id = 0;
    m_unit.m_id = 0;
    m_city.m_id = 0;
    m_selection = SELECTION_TYPE::INACTIVE;
  }
};

namespace selection {
  void initialize();
  const glm::ivec3& get();
  void lclick(const glm::ivec3& location);
  void rclick(const glm::ivec3& location);
  const Selection& get_selection();
}
