#include "sim_interface.h"

#include "4xSimulation/include/step.h"
#include "4xSimulation/include/terminal.h"
#include "4xSimulation/include/simulation.h"
#include "4xSimulation/include/game_types.h"

#include <thread>
#include <mutex>
#include <atomic>

#define SIM_LOG_FILE "sim.log"

namespace sim_interface {
  std::thread s_thread;
  std::mutex s_simmutex;
  std::atomic<bool> s_statechanged (false);
  uint32_t s_currentplayer = 0;
  uint32_t s_playercount = 0;

  world_map::TileMap s_tiles;
  std::vector<Unit> s_units;
  std::vector<City> s_cities;

  bool s_killsim = false;

  void run_sim() {
    sim_interface::synch();

    while (!s_killsim) {
      Step* step = terminal::parse_input();

      // If a kill is invoked simulation has been killed, so don't try to
      // give it a step. Valid or not.
      if (s_killsim) {
        break;
      }

      // Parse input can return nullptr.
      if (!step) continue;

      if (step->m_command == COMMAND_TYPE::QUIT) {
        s_killsim = true;
      }

      {
        std::lock_guard<std::mutex> lock(s_simmutex);
        simulation::process_step(step);
        s_statechanged = true;
      }

    }

    simulation::kill();
    terminal::kill();
  }
}

void sim_interface::initialize() {
  simulation::start();
  terminal::initialize();
  s_thread = std::thread(&run_sim);
}

void sim_interface::move_unit(uint32_t id, const glm::ivec3& location) {
  std::lock_guard<std::mutex> lock(s_simmutex);
  // Find unit with the id.
  for (const auto& u : s_units) {
    // Move it.
    if (u.m_unique_id == id) {
      MoveStep* m = new MoveStep();
      m->m_unit_id = id;
      m->m_destination = sf::Vector3i(location.x, location.y, location.z);
      m->m_player = u.m_owner_id;
      simulation::process_step(m);
      s_statechanged = true;
      delete m;
    }
  }
}

void sim_interface::end_turn() {
  std::lock_guard<std::mutex> lock(s_simmutex);
  EndTurnStep* end = new EndTurnStep();
  end->m_player = s_currentplayer;
  ++s_currentplayer;
  // Cycle
  if (s_currentplayer == s_playercount) {
    s_currentplayer = 0;
  }
  end->m_next_player = s_currentplayer;
  simulation::process_step(end);
  s_statechanged = true;
  delete end;
}

void sim_interface::join_player() {
  std::lock_guard<std::mutex> lock(s_simmutex);
  AddPlayerStep* player = new AddPlayerStep();
  player->m_name = "player" + std::to_string(s_playercount);
  player->ai_type = AI_TYPE::HUMAN;
  simulation::process_step(player);
  s_statechanged = true;
  ++s_playercount;
  delete player;
}

void sim_interface::join_barbarian() {
  std::lock_guard<std::mutex> lock(s_simmutex);
  AddPlayerStep* barbarian = new AddPlayerStep();
  barbarian->m_name = "barbarian" + std::to_string(s_playercount);
  barbarian->ai_type = AI_TYPE::BARBARIAN;
  simulation::process_step(barbarian);
  s_statechanged = true;
  ++s_playercount;
  delete barbarian;
}

void sim_interface::teardown() {
  s_killsim = true;
  s_thread.join();
}

const world_map::TileMap& sim_interface::get_map() {
  return s_tiles;
}

const std::vector<Unit>& sim_interface::get_units() {
  return s_units;
}

const std::vector<City>& sim_interface::get_cities() {
  return s_cities;
}

void sim_interface::synch() {
  std::lock_guard<std::mutex> lock(s_simmutex);
  s_tiles = world_map::get_map();
  s_units.clear();
  auto append_unit = [](const Unit& unit) {
    s_units.push_back(unit);
  };
  units::for_each_unit(append_unit);
  s_cities.clear();
  auto append_city = [](const City& city) {
    s_cities.push_back(city);
  };
  city::for_each_city(append_city);
  s_statechanged = false;
}

bool sim_interface::poll() {
  return s_statechanged;
}
