#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

int main() {
  if (!glfwInit()) {
    std::cout << "Error: could not start glfw3\n" << std::endl;
    return 1;
  }

  GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
  if (!window) {
    std::cout << "Error: could not open window with glfw3\n" << std::endl;
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  gl3wInit();

  const GLubyte* renderer = glGetString(GL_RENDERER); // Get renderer string
  const GLubyte* version = glGetString(GL_VERSION);   // Version as a string

  std::cout << "renderer: " << renderer << std::endl;
  std::cout << "OpenGL verion supported: " << version << std::endl;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

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

  const char* vertex_shader =
  "#version 400\n"
  "in vec3 vp;"
  "void main() {"
  "  gl_Position = vec4(vp, 1.0);"
  "}";

  const char* fragment_shader =
  "#version 400\n"
  "out vec4 frag_color;"
  "void main() {"
  "  frag_color = vec4(0.5, 0.0, 0.5, 1.0);"
  "}";

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);

  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, fs);
  glAttachShader(shader_program, vs);
  glLinkProgram(shader_program);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}
