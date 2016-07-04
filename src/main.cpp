#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include "shader.h"
#include "log.h"
#include "gl.h"

int main() {
  GLFWwindow* window = gl::initialize("Hello Triangle", false);
  if (!window) {
    std::cout << "Failed to initialize gl context. See logs." << std::endl;
    return 1;
  }

  float points[] = {
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
  };

  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  GLuint vs = shader::compile_from_buffer(GL_VERTEX_SHADER, shader::BASIC_V);
  GLuint fs = shader::compile_from_buffer(GL_FRAGMENT_SHADER, shader::BASIC_F);
  GLuint program = shader::link({ vs, fs });

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwPollEvents();
    glfwSwapBuffers(window);

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, 1);
    }
  }

  glfwTerminate();

  return 0;
}
