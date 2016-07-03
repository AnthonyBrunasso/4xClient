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

  glfwTerminate();

  return 0;
}
