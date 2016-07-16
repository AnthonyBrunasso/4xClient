#include <iostream>
#include <vector>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "geometry.h"
#include "log.h"
#include "gl.h"
#include "map.h"
#include "mesh.h"
#include "sim_interface.h"
#include "tiny_obj_loader.h"

int main() {
  GLFWwindow* window = gl::initialize("Hello Triangle", false);
  if (!window) {
    std::cout << "Failed to initialize gl context. See logs." << std::endl;
    return 1;
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  // Camera must be built before the map is initialized.
  Camera camera(0.1f, 200.0f, 45.0f, static_cast<float>(width) / height);

  Mesh* m = mesh::create(glm::vec3(0.0f, 0.0f, 0.3f), "bunny.obj", {
    {GL_VERTEX_SHADER, "simple_phong.vert"},
    {GL_FRAGMENT_SHADER, "simple_phong.frag"}
  });

  mesh::set_scale(m, glm::vec3(5.0f, 5.0f, 5.0f));
  mesh::set_rotate(m, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

  sim_interface::start();
  map::initialize();

  while (!glfwWindowShouldClose(window)) {
    static double previous_seconds = glfwGetTime();
    double current_seconds = glfwGetTime();
    double delta_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;

    map::update(delta_seconds);
    camera.update(delta_seconds);

    // Draw and stuff.
    gl::update_fps_counter(window);
    // Get framebuffer size.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.75f, 1.0f);
    glViewport(0, 0, width, height);

    mesh::draw(m);
    map::draw();

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

  glfwTerminate();
  sim_interface::kill();

  return 0;
}
