#include "sim_interface.h"

#include "terminal.h"
#include "simulation.h"
#include "game_types.h"
#include "network_types.h"
#include "e4x/src/socket/include/messaging.h"
#include "selection.h"
#include "e4x/include/client_util.h"

#include <chrono>
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
  uint32_t s_localplayer = -1;
  uint32_t s_currentturn = 0;
  std::vector<uint32_t> s_barbarians;
  std::vector<uint32_t> s_monsters;

  world_map::TileMap s_tiles;
  std::vector<Unit> s_units;
  std::vector<City> s_cities;
  std::vector<Player> s_players;
  std::unordered_map<uint32_t, NotificationVector > s_notifications;
  std::vector<std::function<void()> > s_subs;

  std::atomic<bool> s_killsim (false);
  std::atomic<bool> s_multiplayer(false);

  constexpr const size_t BUFFER_LEN = largest_message();
  char s_buffer[BUFFER_LEN];

  template<typename T>
  size_t simulate_step(const T& step) {
    std::lock_guard<std::mutex> lock(s_simmutex);
    uint32_t bytes = serialize(s_buffer, BUFFER_LEN, step);

    xmessaging::queue(s_buffer, bytes);
    return bytes;
  }

  template<typename T>
  size_t initiate_step(const T& step) {
    if (!is_me(step.get_player())) {
      std::cout << "Cannot initiate a step on another player's behalf. Local player: " << s_localplayer << ". Step requested player: " << step.get_player() << std::endl;
      return 0;
    }
    return simulate_step(step);
  }

  void run_messaging() {
    
    while (!s_killsim) {
      std::this_thread::sleep_for(std::chrono::milliseconds(16));
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
      size_t bytes = terminal::step_to_bytes(tokens, s_buffer, BUFFER_LEN);
      if (!bytes) continue;
      NETWORK_TYPE t = read_type(s_buffer, bytes);
      s_killsim = t == NETWORK_TYPE::QUITSTEP;

      xmessaging::queue(s_buffer, bytes);
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
  std::cout << "Client's protocol version is: " << get_checksum() << std::endl;
  if (multiplayer == MULTIPLAYER::YES) {
    xmessaging::init_transport("192.168.0.108", 4000);
  }
  s_consumer_thread = std::thread(&run_messaging);
  s_input_thread = std::thread(&run_sim);
}

void sim_interface::move_unit(uint32_t id, const glm::ivec3& location) {
  // Find unit with the id.
  Unit searchTarget(id);
  const Unit* unit = util::c_binsearch(&searchTarget, s_units.data(), s_units.size());
  if (!unit) return;
  MoveStep m;
  m.set_unit_id(id);
  m.set_destination(sf::Vector3i(location.x, location.y, location.z));
  m.set_player(unit->m_owner_id);
  m.set_immediate(true);
  initiate_step(m);
}

void sim_interface::construct(uint32_t city_id, CONSTRUCTION_TYPE type) {
  ConstructionStep construction;
  construction.set_city_id(city_id);
  construction.set_production_id(static_cast<uint32_t>(type));
  construction.set_player(s_currentplayer);
  initiate_step(construction);
}

void sim_interface::specialize(uint32_t city_id, TERRAIN_TYPE type) {
  SpecializeStep s;
  s.set_player(s_currentplayer);
  s.set_city_id(city_id);
  s.set_terrain_type(static_cast<uint32_t>(type));
  initiate_step(s);
} 

void sim_interface::end_turn() {
  EndTurnStep step;

  auto findbarb = std::find(s_barbarians.begin(), s_barbarians.end(), s_currentplayer);
  auto findmonster = std::find(s_monsters.begin(), s_monsters.end(), s_currentplayer);
  if (findbarb == s_barbarians.end() && findmonster == s_monsters.end() && !is_me(s_currentplayer)) return;

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
  if (s_localplayer != -1) return;

  AddPlayerStep player;
  s_localplayer = s_playercount;
  std::cout << "Joining player " << std::to_string(s_playercount) << std::endl;
  player.set_name("player" + std::to_string(s_playercount));
  player.set_ai_type(AI_TYPE::HUMAN);
  simulate_step(player);
}

void sim_interface::resume_player() {
  if (s_localplayer != -1) return;

  s_localplayer = s_currentplayer;
}

void sim_interface::join_barbarian() {
  AddPlayerStep barbarian;
  barbarian.set_name("barbarian" + std::to_string(s_playercount));
  barbarian.set_ai_type(AI_TYPE::BARBARIAN);
  simulate_step(barbarian);
}

bool sim_interface::is_me(uint32_t player) {
  if (s_localplayer == -1) return false;
  return player == s_localplayer;
}

void sim_interface::initial_settle() {
  SpawnStep spawn;
  spawn.set_unit_type(static_cast<uint32_t>(UNIT_TYPE::WORKER));
  spawn.set_player(s_currentplayer);
  const glm::vec3& l = selection::get();
  spawn.set_location(sf::Vector3i(l.x, l.y, l.z));
  initiate_step(spawn);
  settle();
}

void sim_interface::settle() {
  ColonizeStep colonize;
  colonize.set_player(s_currentplayer);
  const glm::vec3& l = selection::get();
  colonize.set_location(sf::Vector3i(l.x, l.y, l.z));
  initiate_step(colonize);
}

void sim_interface::production_abort(uint32_t city, uint32_t index) {
  ProductionAbortStep abort;
  abort.set_player(s_currentplayer);
  abort.set_city(city);
  abort.set_index(index);
  initiate_step(abort);
}

void sim_interface::production_move(uint32_t city, uint32_t source, uint32_t dest) {
  ProductionMoveStep move;
  move.set_player(s_currentplayer);
  move.set_city(city);
  move.set_source_index(source);
  move.set_destination_index(dest);
  initiate_step(move);
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

void sim_interface::attack(uint32_t from_id, uint32_t to_id) {
  AttackStep attack_step;
  attack_step.set_attacker_id(from_id);
  attack_step.set_defender_id(to_id);
  attack_step.set_player(s_currentplayer);
  initiate_step(attack_step);
}

void sim_interface::siege(uint32_t unit_id, uint32_t city_id) {
  SiegeStep s;
  s.set_city(city_id);
  s.set_unit(unit_id);
  s.set_player(s_currentplayer);
  initiate_step(s);
}

void sim_interface::cast(const glm::ivec3& loc, MAGIC_TYPE type) {
  MagicStep m;
  m.set_type(type);
  m.set_location(sf::Vector3i(loc.x, loc.y, loc.z));
  m.set_player(s_currentplayer);
  initiate_step(m);
}

void sim_interface::harvest(const glm::ivec3& loc) {
  HarvestStep h;
  h.set_destination(sf::Vector3i(loc.x, loc.y, loc.z));
  h.set_player(s_currentplayer);
  initiate_step(h);
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

const NotificationVector& sim_interface::get_player_notifications(uint32_t player_id) {
  return s_notifications[player_id];
}

void sim_interface::synch() {
  std::lock_guard<std::mutex> lock(s_simmutex);
  
  s_currentplayer = 0;
  while (true) {
    Player* p = player::get_player(s_currentplayer);
    if (!p) break;
    if (p->m_turn_state == TURN_TYPE::TURNACTIVE) break;
    ++s_currentplayer;
    // Cycle
    if (s_currentplayer >= s_playercount) {
      s_currentplayer = 0;
    }
  };
  
  std::vector<uint32_t> barbarians, monsters;
  player::for_each_player([&barbarians, &monsters](const Player& p) {
    switch (p.m_ai_type) {
      case AI_TYPE::BARBARIAN:
        barbarians.push_back(p.m_id);
        break;
      case AI_TYPE::MONSTER:
        monsters.push_back(p.m_id);
        break;
      case AI_TYPE::HUMAN:
      default:
        break;
    }
  });
  s_barbarians.swap(barbarians);
  s_monsters.swap(monsters);
  
  s_tiles = world_map::get_map();
  s_units.clear();
  auto append_unit = [](const Unit& unit) {
    s_units.push_back(unit);
  };
  unit::for_each_unit(append_unit);
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
  s_currentturn = simulation::get_turn();

  for (uint32_t i = 0; i < s_playercount; ++i) {
    s_notifications[i] = notification::get_player_notifications(i);
  }

  // Sort all vectors by unique id for fast lookup.

  // Players are sorted by id in 4xsim but lets not assume that and sort it here anyway. 
  util::c_qsort(s_players.data(), s_players.size());
  util::c_qsort(s_cities.data(), s_cities.size());
  util::c_qsort(s_units.data(), s_units.size());

  // Notify subscribers the sim has been synched.
  for (const auto& s : s_subs) s();

  s_statechanged = false;
}

void sim_interface::sub_synch(std::function<void()> sub) {
  s_subs.push_back(sub);
}

bool sim_interface::poll() {
  return s_statechanged;
}

uint32_t sim_interface::get_currentplayer() {
  return s_currentplayer;
}

uint32_t sim_interface::get_currentturn() {
  return s_currentturn;
}

const Player* sim_interface::get_currentplayerptr() {
  if (s_players.size() - 1 < s_currentplayer) {
    return nullptr;
  }

  return &s_players[s_currentplayer];
}
