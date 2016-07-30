#include <iostream>
#include <vector>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include "camera.h"
#include "gl.h"
#include "map.h"
#include "mesh.h"
#include "sim_interface.h"
#include "program.h"
#include "light.h"
#include "ui.h"

void initialize() {
  program::build("phong", {
    {GL_VERTEX_SHADER, "simple_phong.vert"},
    {GL_FRAGMENT_SHADER, "simple_phong.frag"},
  });

  glm::vec3 pos(0.0, 0.0f, 0.0f);
  light::add(pos);
}

int main() {
  GLFWwindow* window = gl::initialize("Hello Triangle", false, 1280, 720);
  if (!window) {
    std::cout << "Failed to initialize gl context. See logs." << std::endl;
    return 1;
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  initialize();

  // Camera must be built before the map is initialized.
  Camera camera(0.1f, 200.0f, 45.0f, static_cast<float>(width) / height);

  ImGui_ImplGlfwGL3_Init(window, true);
  ui::debug(true);
  bool show_test_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImColor(114, 144, 154);

  sim_interface::initialize();
  map::initialize();

  while (!glfwWindowShouldClose(window)) {
    static double previous_seconds = glfwGetTime();
    double current_seconds = glfwGetTime();
    double delta_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;

    ImGui_ImplGlfwGL3_NewFrame();

    /*{
      static float f = 0.0f;
      ImGui::Text("Hello, world!");
      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
      ImGui::ColorEdit3("clear color", (float*)&clear_color);
      if (ImGui::Button("Test Window")) show_test_window ^= 1;
      if (ImGui::Button("Another Window")) show_another_window ^= 1;
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (show_another_window)
    {
      ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
      ImGui::Begin("Another Window", &show_another_window);
      ImGui::Text("Hello");
      ImGui::End();
    }*/

    //ImGui::ShowTestWindow();

    ui::update();
    map::update(delta_seconds);
    camera.update(delta_seconds);

    // Draw and stuff.
    gl::update_fps_counter(window);
    // Get framebuffer size.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glViewport(0, 0, width, height);

    //mesh::draw(m);
    map::draw();
    ImGui::Render();

    glfwPollEvents();
    glfwSwapBuffers(window);

    // Recompile shaders
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_R)) {
      //mesh.reset_shaders();
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, 1);
    }
  }
  
  // Don't commit if there is a gl error.
  assert(!glGetError());

  ImGui_ImplGlfwGL3_Shutdown();
  glfwTerminate();
  //delete m;
  map::teardown();
  sim_interface::teardown();

  return 0;
}
