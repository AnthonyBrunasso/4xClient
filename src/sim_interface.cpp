#include "sim_interface.h"

#include "4xSimulation/include/step.h"
#include "4xSimulation/include/terminal.h"
#include "4xSimulation/include/simulation.h"
#include "4xSimulation/include/game_types.h"

#include <thread>
#include <mutex>

#define SIM_LOG_FILE "sim.log"

namespace sim_interface {
  std::thread s_thread;
  std::mutex s_simmutex;

  bool s_killsim = false;

  void run_sim() {
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

void sim_interface::teardown() {
  s_killsim = true;
  s_thread.join();
}

world_map::TileMap sim_interface::get_map() {
  std::lock_guard<std::mutex> lock(s_simmutex);
  return world_map::get_map();
}
