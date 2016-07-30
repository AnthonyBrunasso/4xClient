#include "ui.h"

#include <imgui.h>

#include "circular_buffer.h"
#include "sim_interface.h"
#include "mesh.h"

namespace ui {
  // Cities to render ui for.
  std::vector<uint32_t> s_cities;
  CBuffer<float> s_framehistory(1000);
  bool s_debug = false;

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
    ImGui::PlotLines("", 
      s_framehistory.data(), 
      s_framehistory.size(), 
      1, 
      "", 
      0, 
      75.0f,
      ImVec2(0, 80));
    ImGui::End();
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
