#include "gl.h"
#include "log.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <sstream>

#define GL_LOG_FILE "gl.log"

void glfw_error_callback(int error, const char* description) {
  std::ostringstream ss;

  ss << "GLFW ERROR - code: " << error 
     << " description: " << description << std::endl;

  logging::write(GL_LOG_FILE, ss.str());
}

GLFWwindow* gl::initialize(int width, int height, const char* title) {
  std::ostringstream ss;
  ss << "Initializing OpenGL window with width: "
     << width << " height: "
     << height << " title: "
     << title << std::endl;

  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit()) {
    ss << "Error: Failed glfwInit()" << std::endl;
    logging::write(GL_LOG_FILE, ss.str());
    return nullptr;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  // Anti-aliasing
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


  GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window) {
    ss << "Error: Could not open window with glfw3" << std::endl;
    glfwTerminate();
    logging::write(GL_LOG_FILE, ss.str());
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  gl3wInit();

  const GLubyte* renderer = glGetString(GL_RENDERER); // Get renderer string
  const GLubyte* version = glGetString(GL_VERSION);   // Version as a string

  ss << "Renderer: " << renderer << std::endl;
  ss << "OpenGL verion supported: " << version << std::endl;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  logging::write(GL_LOG_FILE, ss.str());

  return window;
}
