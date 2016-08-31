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
#include "prop.h"

void initialize() {
  program::build("phong", {
    {GL_VERTEX_SHADER, "simple_phong.vert"},
    {GL_FRAGMENT_SHADER, "simple_phong.frag"},
  });

  program::build("outline", {
    {GL_VERTEX_SHADER, "simple_phong.vert"},
    {GL_FRAGMENT_SHADER, "outline.frag"},
  });

  glm::vec3 pos(0.0, 30.0f, 10.0f);
  light::add(pos);
  pos = glm::vec3(0.0, -30.0f, 10.0f);
  light::add(pos);
}

int main() {
  GLFWwindow* window = gl::initialize("Hello 4xClient!", false, 1280, 720);
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

  sim_interface::initialize(MULTIPLAYER::NO);
  map::initialize();
  prop::initialize();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    static double previous_seconds = glfwGetTime();
    double current_seconds = glfwGetTime();
    double delta_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;

    ImGui_ImplGlfwGL3_NewFrame();

    ui::update();
    map::update(delta_seconds);
    camera.update(delta_seconds);

    // Draw and stuff.
    gl::update_fps_counter(window);
    // Get framebuffer size.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glViewport(0, 0, width, height);

    map::draw();
    prop::render();
    ImGui::Render();

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
