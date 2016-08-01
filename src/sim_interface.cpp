#include "sim_interface.h"

#include "terminal.h"
#include "simulation.h"
#include "game_types.h"
#include "network_types.h"
#include "messaging.h"
#include "selection.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>
#include <string>
#include <vector>


#define SIM_LOG_FILE "sim.log"

namespace sim_interface {
  std::thread s_input_thread;
  std::thread s_consumer_thread;
  std::mutex s_simmutex;
  std::atomic<bool> s_statechanged (true);
  uint32_t s_currentplayer = 0;
  uint32_t s_playercount = 0;

  world_map::TileMap s_tiles;
  std::vector<Unit> s_units;
  std::vector<City> s_cities;
  std::vector<Player> s_players;

  std::atomic<bool> s_killsim (false);
  std::atomic<bool> s_multiplayer(false);

  const size_t BUFFER_LEN = largest_message();
  std::vector<char> s_buffer(BUFFER_LEN);

  template<typename T>
  size_t simulate_step(const T& step) {
    std::lock_guard<std::mutex> lock(s_simmutex);
    uint32_t bytes = serialize(s_buffer.data(), BUFFER_LEN, step);

    xmessaging::queue(s_buffer.data(), bytes);
    return bytes;
  }

  void run_messaging() {
    
    while (!s_killsim) {
      // Hold the simulation mutex while we pump the network
      std::lock_guard<std::mutex> lock(s_simmutex);
      bool message_read = false;
      char *buffer;
      size_t buffer_len;
      xmessaging::update(message_read, buffer, buffer_len);
      if (message_read)
      {
        simulation::process_step(buffer, buffer_len);
        s_statechanged = true;
      }
    }
  }

  void run_sim() {
    
    while (!s_killsim) {
      std::string input;
      std::cout << ">";
      std::getline(std::cin, input);
      if (!std::cin.good() || s_killsim) {
        break;
      }

      std::vector<std::string> tokens = terminal::tokenize(input);
      if (tokens.empty()) continue;

      if (terminal::is_query(tokens)) {
        std::lock_guard<std::mutex> lock(s_simmutex);
        terminal::run_query(tokens);
        continue;
      }

      std::lock_guard<std::mutex> lock(s_simmutex);
      size_t bytes = terminal::step_to_bytes(tokens, s_buffer.data(), BUFFER_LEN);
      if (!bytes) continue;
      NETWORK_TYPE t = read_type(s_buffer.data(), bytes);
      s_killsim = t == NETWORK_TYPE::QUITSTEP;

      xmessaging::queue(s_buffer.data(), bytes);
    }

    terminal::kill();
    simulation::kill();
  }
}

void sim_interface::initialize(MULTIPLAYER multiplayer) {
  simulation::start();
  terminal::initialize();

  xmessaging::alloc_read_buffer(largest_message());
  s_multiplayer = multiplayer == MULTIPLAYER::YES;
  if (multiplayer == MULTIPLAYER::YES) {
    xmessaging::init_transport("rufeooo.com", 4000);
    std::cout << "Client's protocol version is: " << get_checksum() << std::endl;
  }
  s_consumer_thread = std::thread(&run_messaging);
  s_input_thread = std::thread(&run_sim);
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

void sim_interface::construct(uint32_t city_id, CONSTRUCTION_TYPE type) {
  ConstructionStep construction;
  construction.set_city_id(city_id);
  construction.set_production_id(static_cast<uint32_t>(type));
  construction.set_player(s_currentplayer);
  simulate_step(construction);
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
  if (s_multiplayer) {
    xmessaging::stop_transport();
  }
  xmessaging::dealloc_read_buffer();
  s_consumer_thread.join();
  s_input_thread.join();
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

  // Sort all vectors by unique id for fast lookup.

  // Players are sorted by id in 4xsim but lets not assume that and sort it here anyway. 
  std::sort(s_players.begin(), s_players.end(), [](const Player& lhs, const Player& rhs) {
    return rhs.m_id > lhs.m_id;
  });

  std::sort(s_cities.begin(), s_cities.end(), [](const City& lhs, const City& rhs) {
    return rhs.m_id > lhs.m_id;
  });

  std::sort(s_units.begin(), s_units.end(), [](const Unit& lhs, const Unit& rhs) {
    return rhs.m_unique_id > lhs.m_unique_id;
  });

  s_statechanged = false;
}

bool sim_interface::poll() {
  return s_statechanged;
}
