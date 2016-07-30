#include "ui.h"

#include <imgui.h>
#include <iostream>

#include "circular_buffer.h"
#include "sim_interface.h"
#include "mesh.h"
#include "log.h"

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
}

void ui::debug(bool on) {
  s_debug = on;
}

void ui::city(uint32_t id) {
  s_cities.push_back(id);
}

void ui::update() {
  if (s_debug) debug_ui();


}
