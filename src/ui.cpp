#include "ui.h"

#include <imgui.h>
#include <iostream>
#include <algorithm>

#include "circular_buffer.h"
#include "production.h"
#include "sim_interface.h"
#include "mesh.h"
#include "log.h"
#include "util.h"

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

    ImGui::ShowTestWindow();
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
  auto city_compare = [](uint32_t id, const City& rhs) {
    return id > rhs.m_id;
  };
  const std::vector<City>& cities = sim_interface::get_cities(); 
  std::vector<uint32_t> cities_to_close;
  // Show city ui for selected cities.
  for (auto id : s_cities) {
    const City* city = util::id_binsearch(cities.data(), cities.size(), id, city_compare);
    if (!city) continue;
    ImGui::Begin("City");
    ImGui::Text("Production");
    ImGui::Separator();
    ConstructionQueueFIFO* construction = city->GetConstruction();
    if (construction && !construction->Queue().empty()) {
      ImGui::Text("Constructing: %s", get_construction_name(construction->Queue().front()));
      ImGui::ProgressBar(construction->m_queue.front()->m_production / production::required(construction->Queue().front()), ImVec2(0.0f, 0.0f));
    }
    else {
      ImGui::Text("No current production");
    }

    if (construction && construction->Queue().size() > 1) {
      ImGui::Text("Queue");
      ImGui::Columns(2, "mycolumns"); // 4-ways, with border
      ImGui::Separator();
      ImGui::Text("Construction"); ImGui::NextColumn();
      ImGui::Text("Production"); ImGui::NextColumn();
      ImGui::Separator();

      for (uint32_t i = 1; i < construction->Queue().size(); ++i) {
        ImGui::Text("%s", get_construction_name(construction->Queue()[i])); ImGui::NextColumn();
        ImGui::Text("%.2f", production::required(construction->Queue()[i])); ImGui::NextColumn();
      }

      ImGui::Columns(1);
      ImGui::Separator();
    }

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
