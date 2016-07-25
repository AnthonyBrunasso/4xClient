#include "sim_interface.h"

#include "terminal.h"
#include "simulation.h"
#include "game_types.h"
#include "network_types.h"
#include "selection.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>
#include <string>

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
  std::vector<Player> s_players;

  std::atomic<bool> s_killsim (false);

  const size_t BUFFER_LEN = 256;
  char s_buffer[BUFFER_LEN];

  template<typename T>
  size_t simulate_step(const T& step) {
    std::lock_guard<std::mutex> lock(s_simmutex);
    size_t bytes = serialize(s_buffer, BUFFER_LEN, step);
    simulation::process_step(s_buffer, bytes);
    s_statechanged = true;
    std::cout << ">";
    return bytes;
  }

  void run_sim() {
    sim_interface::synch();

    while (!s_killsim) {
      std::string value;
      std::cout << ">";
      std::getline(std::cin, value);
      if (!std::cin.good() || s_killsim) {
        break;
      }

      {
        std::lock_guard<std::mutex> lock(s_simmutex);
        // parse_input will return false when quit has been called
        if (!terminal::parse_input(value)) {
          break;
        }
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
  // Find unit with the id.
  for (const auto& u : s_units) {
    // Move it.
    if (u.m_unique_id == id) {
      MoveStep m;
      m.set_unit_id(id);
      m.set_destination(sf::Vector3i(location.x, location.y, location.z));
      m.set_player(u.m_owner_id);
      m.set_immediate(true);
      simulate_step(m);
    }
  }
}

void sim_interface::end_turn() {
  EndTurnStep step;
  step.set_player(s_currentplayer);
  ++s_currentplayer;
  // Cycle
  if (s_currentplayer >= s_playercount) {
    s_currentplayer = 0;
  }
  step.set_next_player(s_currentplayer);
  simulate_step(step);
}

void sim_interface::join_player() {
  AddPlayerStep player;
  std::cout << "Joining player " << std::to_string(s_playercount) << std::endl;
  player.set_name("player" + std::to_string(s_playercount));
  player.set_ai_type(AI_TYPE::HUMAN);
  simulate_step(player);
}

void sim_interface::join_barbarian() {
  AddPlayerStep barbarian;
  barbarian.set_name("barbarian" + std::to_string(s_playercount));
  barbarian.set_ai_type(AI_TYPE::BARBARIAN);
  simulate_step(barbarian);
}

void sim_interface::initial_settle() {
  SpawnStep spawn;
  spawn.set_unit_type(static_cast<uint32_t>(UNIT_TYPE::WORKER));
  spawn.set_player(s_currentplayer);
  const glm::vec3& l = selection::get();
  spawn.set_location(sf::Vector3i(l.x, l.y, l.z));
  simulate_step(spawn);
  settle();
}

void sim_interface::settle() {
  ColonizeStep colonize;
  colonize.set_player(s_currentplayer);
  const glm::vec3& l = selection::get();
  colonize.set_location(sf::Vector3i(l.x, l.y, l.z));
  simulate_step(colonize);
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

const std::vector<Player>& sim_interface::get_players() {
  return s_players;
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
  s_players.clear();
  auto append_player = [](const Player& player) {
    s_players.push_back(player);
  };
  player::for_each_player(append_player);
  s_playercount = player::get_count();
  s_statechanged = false;
}

bool sim_interface::poll() {
  return s_statechanged;
}
