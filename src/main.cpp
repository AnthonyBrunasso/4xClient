#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

  //glm::mat4 matrix(1.0);

  Mesh mesh(glm::vec3(0.0f, 0.0f, 0.0f), 
      geometry::get_hexagon2d(), 
      geometry::get_triangle_color(),
      {
        {GL_VERTEX_SHADER, "simple_transform.vert"}, 
        {GL_FRAGMENT_SHADER, "simple.frag"}
      });

  mesh.initialize();

  float speed = 1.0f;
  float last_position = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    static double previous_seconds = glfwGetTime();
    double current_seconds = glfwGetTime();
    double delta_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;
    mesh.update(delta_seconds);

    //if (fabs(last_position) > 1.0f) {
    //  speed *= -1;
   // }

    //matrix[3][0] = delta_seconds * speed + last_position;
    //last_position = matrix[3][0];
    //glUseProgram(program);
    //glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, glm::value_ptr(matrix));*/

    // Draw and stuff
    gl::update_fps_counter(window);
    // Get framebuffer size.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glViewport(0, 0, width, height);
    mesh.draw();
    //glBindVertexArray(vao);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
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
