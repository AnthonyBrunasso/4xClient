#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include "shader.h"
#include "geometry.h"
#include "log.h"
#include "gl.h"

int main() {
  GLFWwindow* window = gl::initialize("Hello Triangle", false);
  if (!window) {
    std::cout << "Failed to initialize gl context. See logs." << std::endl;
    return 1;
  }

  GLuint points_vbo = 0;
  glGenBuffers(1, &points_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glBufferData(GL_ARRAY_BUFFER, 
    geometry::get_triangle().size() * sizeof(GLfloat), 
    geometry::get_triangle().data(), 
    GL_STATIC_DRAW);

  GLuint colors_vbo = 0;
  glGenBuffers(1, &colors_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
  glBufferData(GL_ARRAY_BUFFER, 
    geometry::get_triangle_color().size() * sizeof(GLfloat),
    geometry::get_triangle_color().data(),
    GL_STATIC_DRAW);

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);


  GLuint vs = shader::compile_from_file(GL_VERTEX_SHADER, "simple_color.vert");
  GLuint fs = shader::compile_from_file(GL_FRAGMENT_SHADER, "simple_color.frag");
  GLuint program = shader::link({ vs, fs });

  while (!glfwWindowShouldClose(window)) {
    gl::update_fps_counter(window);
    // Get framebuffer size.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glViewport(0, 0, width, height);
    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwPollEvents();
    glfwSwapBuffers(window);

    // Recompile shaders
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_R)) {
      // Delete program and shaders
      glDeleteProgram(program);
      glDeleteShader(vs);
      glDeleteShader(fs);

      // Recompile and relink
      vs = shader::compile_from_file(GL_VERTEX_SHADER, "simple.vert");
      fs = shader::compile_from_file(GL_FRAGMENT_SHADER, "simple.frag");
      program = shader::link({ vs, fs });
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, 1);
    }
  }

  glfwTerminate();

  return 0;
}
