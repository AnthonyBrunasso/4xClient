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
  std::atomic<bool> s_statechanged = false;

  world_map::TileMap s_tiles;
  std::vector<Unit> s_units;

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
      delete m;
    }
  }
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

void sim_interface::synch() {
  std::lock_guard<std::mutex> lock(s_simmutex);
  s_tiles = world_map::get_map();
  s_units.clear();
  auto append_unit = [](const Unit& unit) {
    s_units.push_back(unit);
  };
  units::for_each_unit(append_unit);
  s_statechanged = true;
}

bool sim_interface::poll() {
  return s_statechanged;
}
