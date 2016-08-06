#include "ui.h"

#include <imgui.h>
#include <iostream>
#include <algorithm>

#include "circular_buffer.h"
#include "production.h"
#include "sim_interface.h"
#include "mesh.h"
#include "log.h"
#include "client_util.h"
#include "selection.h"
#include "unit_definitions.h"
#include "notification.h"
#include "terrain_yield.h"
#include "prop.h"
#include "map.h"

namespace ui {
  // Cities to render ui for.
  std::vector<uint32_t> s_cities;
  CBuffer<float> s_framehistory(1000);
  bool s_debug = false;

  void debug_log(const std::string& title, const std::string& log, bool& show) {
    ImGui::Begin(title.data(), &show);
    ImGui::Text(log.data());
    ImGui::End();
  }

  void debug_ui() {
    static float fps;
    fps = ImGui::GetIO().Framerate;
    s_framehistory.insert(fps);
    ImGui::Begin("Debug");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
      1000.0f / fps, fps);


    ImGui::Text("Vertex count: %d", mesh::get_vert_count());
    ImGui::Text("Unit count: %d", sim_interface::get_units().size());
    ImGui::Text("City count: %d", sim_interface::get_cities().size());
    ImGui::Text("Player count: %d", sim_interface::get_players().size());

    ImGui::Text("Frame rate circular buffer:");
    ImGui::PlotLines("Buffer", 
      s_framehistory.data(), 
      s_framehistory.size(), 
      1, 
      "", 
      0, 
      75.0f,
      ImVec2(0, 80));

    static bool show_shader_logs = false;
    static bool show_gl_logs = false;
    static bool show_mesh_logs = false;
    static bool show_image_logs = false;

    static std::string shader_logs, gl_logs, mesh_logs, image_logs;

    if (ImGui::Button("Shader Logs")) {
      show_shader_logs = true;
      shader_logs = logging::read("shader.log");
    };
    ImGui::SameLine();
    if (ImGui::Button("GL Logs")) {
      show_gl_logs = true;
      gl_logs = logging::read("gl.log");
    };
    ImGui::SameLine();
    if (ImGui::Button("Mesh Logs")) {
      show_mesh_logs = true;
      mesh_logs = logging::read("mesh.log");
    };
    ImGui::SameLine();
    if (ImGui::Button("Image Logs")) {
      show_image_logs = true;
      image_logs = logging::read("image.log");
    };
    ImGui::End();

    if (show_shader_logs) debug_log("Shader Logs", shader_logs, show_shader_logs);
    if (show_gl_logs) debug_log("GL Logs", gl_logs, show_gl_logs);
    if (show_mesh_logs) debug_log("Mesh Logs", mesh_logs, show_mesh_logs);
    if (show_image_logs) debug_log("Image Logs", image_logs, show_image_logs);

    //ImGui::ShowTestWindow();
  }

  void render_construction(ConstructionQueueFIFO* cq) {
    if (!cq) return;
    if (cq->m_queue.size() == 0) {
      ImGui::Text("No current production");
      return;
    }
    
    ConstructionOrder* co = cq->m_queue.front();
    ImGui::Text("Constructing: %s", production::name(co));
    ImGui::ProgressBar(production::current(co) / production::required(co), ImVec2(0.0f, 0.0f));
    
    ImGui::Separator();

    ImGui::Text("Full Queue");
    ImGui::Columns(2, "mycolumns"); // 4-ways, with border
    ImGui::Text("Name"); ImGui::NextColumn();
    ImGui::Text("Turns"); ImGui::NextColumn();
    ImGui::Separator();

    ConstructionList::const_iterator cit = cq->m_queue.begin();
    float turns = 0.0;
    
    for (auto co : cq->m_queue) {
      turns += production::turns(co);
      ImGui::Text("%s", production::name(co)); ImGui::NextColumn();
      ImGui::Text("%.1f", turns); ImGui::NextColumn();
    }

    ImGui::Columns(1);
  }

  void render_unit_selection() {
    const Selection& s = selection::get_selection();
    float max_health = unit_definitions::get(s.m_unit.m_unit_type)->m_health;
    float progress = s.m_unit.m_combat_stats.m_health / max_health;
    char buf[32];
    sprintf(buf, "%.2f/%.2f", progress * max_health, max_health);
    ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), buf);
    ImGui::SameLine();
    ImGui::Text("Health");
    ImGui::Text("Type: %s", get_unit_name(s.m_unit.m_unit_type));
    ImGui::Text("Actions: %d", s.m_unit.m_action_points);
    ImGui::Text("Attack: %.2f", s.m_unit.m_combat_stats.m_attack);
  }

  void render_harvest_selection() {
    const Selection& s = selection::get_selection();
    prop::spinagon(map::get_hover());
    ImGui::Text("Right-click a tile to harvest.");
  }

  void render_selection() {
    static bool open = true;
    open = true;
    ImGui::Begin("Selection", &open, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos("Selection", ImVec2(0, 0));
    ImGui::Text("Current Player: %d", sim_interface::get_currentplayer());
    const Selection& s = selection::get_selection(); 
    switch (s.m_selection) {
      case SELECTION_TYPE::UNIT:
        render_unit_selection();
        break;
      case SELECTION_TYPE::CITY:
        //render_city_selection();
        break;
      case SELECTION_TYPE::HARVEST:
        render_harvest_selection();
        break;
      case SELECTION_TYPE::INACTIVE:
        ImGui::Text("Nothing selected.");
        break;
    }
    ImGui::End();
  }

  void render_yield(const TerrainYield& y) {
    if (y.m_food > 0.0f) {
      ImGui::Text("Food: +%.1f ", y.m_food);
      ImGui::SameLine();
    }
    if (y.m_science > 0.0f) {
      ImGui::Text("Science: +%.1f ", y.m_science);
      ImGui::SameLine();
    }
    if (y.m_production > 0.0f) {
      ImGui::Text("Production: +%.1f ", y.m_production);
      ImGui::SameLine();
    }
    if (y.m_gold > 0.0f) {
      ImGui::Text("Gold: +%.1f ", y.m_gold);
      ImGui::SameLine();
    }
    if (y.m_experience > 0.0f) {
      ImGui::Text("Xp: +%.1f ", y.m_experience);
      ImGui::SameLine();
    }
    if (y.m_magic > 0.0f) {
      ImGui::Text("Magic: +%.1f ", y.m_magic);
      ImGui::SameLine();
    }
  }

  void render_cities() {
    const std::vector<City>& cities = sim_interface::get_cities(); 
    std::vector<uint32_t> cities_to_close;
    // Show city ui for selected cities.
    for (auto id : s_cities) {
      const City* city = util::id_binsearch(cities.data(), cities.size(), id);
      if (!city) continue;
      ImGui::Begin("City");
      ImGui::Text("Production");
      ImGui::Separator();

      render_construction(city->GetProductionQueue());
      ImGui::Separator();

      ImGui::Text("Pick Construction");
      ImGui::Columns(3, "mycolumns"); // 4-ways, with border
      ImGui::Separator();
      ImGui::Text("Construction"); ImGui::NextColumn();
      ImGui::Text("Production"); ImGui::NextColumn();
      ImGui::Text("Lore"); ImGui::NextColumn();
      ImGui::Separator();
      static int selected = -1;
      static CONSTRUCTION_TYPE construct = CONSTRUCTION_TYPE::UNKNOWN;
      int i = 0;
      for_each_construction_type([&i](CONSTRUCTION_TYPE type) {
        if (type == CONSTRUCTION_TYPE::UNKNOWN) return;
        char label[32];
        sprintf(label, "%s", get_construction_name(type));
        if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns)) {
          selected = i;
          construct = type;
        }
        ImGui::NextColumn();
        ImGui::Text("%.2f", production::required(type)); ImGui::NextColumn();
        ImGui::Text("%s", "None"); ImGui::NextColumn();
        ++i;
      });

      if (construct != CONSTRUCTION_TYPE::UNKNOWN) {
        sim_interface::construct(id, construct);
        construct = CONSTRUCTION_TYPE::UNKNOWN;
      }

      ImGui::Columns(1);
      ImGui::Separator();
      
      if (city->CanSpecialize()) {
        TERRAIN_TYPE type = city->m_specialization;
        // Give player specialization picker
        if (type == TERRAIN_TYPE::UNKNOWN) {
          static const char** tnames = get_terrain_names();
          int begin = 0, item = 0;

          for (int i = 0; i < get_terrain_count(); ++i) {
            if (get_terrain_name(type) == tnames[i]) {
              begin = item = i;
            }
          }

          ImGui::Combo("Specialization", &item, &tnames[0], get_terrain_count());
          if (begin != item) {
            sim_interface::specialize(city->m_id, get_terrain_type(tnames[item]));
          }
        }
        // Otherwise show specialization
        else {
          ImGui::Text("Spec: %s ( ", get_terrain_name(type));
          TerrainYield y = terrain_yield::get_specialization_yield(type);
          ImGui::SameLine();
          render_yield(y);
          ImGui::Text(")");
        }
      }

      if (city->GetPopulation()) {
        if (ImGui::Button("Harvest")) selection::set_selection(SELECTION_TYPE::HARVEST);

        if (city->GetHarvestCount()) {
          ImGui::Columns(2, "harvestColumns");
          ImGui::Separator();
          ImGui::Text("Type"); ImGui::NextColumn();
          ImGui::Text("Yield"); ImGui::NextColumn();
          ImGui::Separator();
        }

        for (size_t i = 0; i < city->GetHarvestCount(); ++i) {
          const std::vector<sf::Vector3i>& ys = city->m_yield_tiles;
          glm::ivec3 loc(ys[i].x, ys[i].y, ys[i].z);
          const world_map::TileMap& wm = sim_interface::get_map();
          const auto& ti = wm.find(ys[i]);
          if (ti == wm.end()) continue;
          TerrainYield yield = terrain_yield::get_base_yield(ti->second.m_terrain_type);
          ImGui::Text("%s", get_terrain_name(ti->second.m_terrain_type)); ImGui::NextColumn();
          render_yield(yield); ImGui::NextColumn();
          // Add rotating hexagon to show the tile is being harvested.
          prop::spinagon(loc);
        }

        ImGui::Columns(1);
        ImGui::Separator();
      }
      
       

      if (ImGui::Button("Close")) cities_to_close.push_back(id);
      ImGui::End();
    }

    for (auto id : cities_to_close) {
      s_cities.erase(std::remove_if(s_cities.begin(), s_cities.end(), 
        [id](uint32_t cid) { 
          return cid == id; 
      }), s_cities.end());
    }
  }
}

void ui::debug(bool on) {
  s_debug = on;
}

void ui::city(uint32_t id) {
  // Don't open this multiple times.
  for (auto cid : s_cities) {
    if (cid == id) return;
  }
  s_cities.push_back(id);
}

void ui::update() {
  if (s_debug) debug_ui();
  
  render_cities();
  render_selection();
}
