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

GLFWwindow* gl::initialize(const char* title, bool fullscreen, int width, int height) {
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

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  // Anti-aliasing
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = nullptr;
  if (fullscreen) {
    // Get your primary monitor if two monitors are enabled.
    GLFWmonitor* mon = glfwGetPrimaryMonitor();
    // Gets monitor data, primary width and height.
    const GLFWvidmode* vmode = glfwGetVideoMode(mon);
    window = glfwCreateWindow(vmode->width, vmode->height, title, mon, NULL);
  }
  else {
    window = glfwCreateWindow(width, height, title, NULL, NULL);
  }

  if (!window) {
    ss << "Error: Could not open window with glfw3" << std::endl;
    glfwTerminate();
    logging::write(GL_LOG_FILE, ss.str());
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  // Log the return but don't treat it as an error, this way we can attempt
  // to get the opengl info below on failure.
  ss << "gl3wInit() returned: " << gl3wInit() << std::endl;

  // Log the gl info.
  ss << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  ss << "OpenGL verion supported: " << glGetString(GL_VERSION) << std::endl;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  logging::write(GL_LOG_FILE, ss.str());

  return window;
}
