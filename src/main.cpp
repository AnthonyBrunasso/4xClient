#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "geometry.h"
#include "log.h"
#include "gl.h"
#include "mesh.h"

int main() {
  GLFWwindow* window = gl::initialize("Hello Triangle", false);
  if (!window) {
    std::cout << "Failed to initialize gl context. See logs." << std::endl;
    return 1;
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  Camera camera(0.1f, 200.0f, 45.0f, static_cast<float>(width) / height);

  Mesh mesh(glm::vec3(0.0f, 0.0f, 0.0f), 
      geometry::get_hexagon2d(), 
      geometry::get_triangle_color(),
      {
        {GL_VERTEX_SHADER, "simple_perspective.vert"}, 
        {GL_FRAGMENT_SHADER, "simple.frag"}
      });

  mesh.initialize();

  auto set_view = [&camera](GLuint program) {
    GLint view_mat_location = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(view_mat_location, 
        1, 
        GL_FALSE, 
        glm::value_ptr(camera.m_view));

    GLint proj_mat_location = glGetUniformLocation(program, "proj");
    glUniformMatrix4fv(proj_mat_location, 
        1, 
        GL_FALSE, 
        glm::value_ptr(camera.m_projection));
  };
  mesh.add_predraw(set_view);

  while (!glfwWindowShouldClose(window)) {
    static double previous_seconds = glfwGetTime();
    double current_seconds = glfwGetTime();
    double delta_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;
    mesh.update(delta_seconds);

    // Draw and stuff.
    gl::update_fps_counter(window);
    // Get framebuffer size.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glViewport(0, 0, width, height);
    camera.update();
    mesh.draw();
    glfwPollEvents();
    glfwSwapBuffers(window);

    // Recompile shaders
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_R)) {
      mesh.reset_shaders();
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, 1);
    }
  }

  glfwTerminate();

  return 0;
}
