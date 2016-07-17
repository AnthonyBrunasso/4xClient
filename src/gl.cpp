#include "gl.h"
#include "log.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <sstream>

#define GL_LOG_FILE "gl.log"

namespace gl {
  GLFWwindow* s_current_window = nullptr;
  // glfw callbacks.
  void glfw_error_callback(int error, const char* description) {
    std::ostringstream ss;

    ss << "GLFW ERROR - code: " << error 
       << " description: " << description << std::endl;

    logging::write(GL_LOG_FILE, ss.str());
  }

  void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
    std::ostringstream ss;
    ss << "Window changed event - new width: " <<  width
       << " new height: " << height;
    logging::write(GL_LOG_FILE, ss.str());
  }

  void glfw_framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
    std::ostringstream ss;
    ss << "Framebuffer changed event - new width: " <<  width
       << " new height: " << height;
    logging::write(GL_LOG_FILE, ss.str());
  }

  // Log gl params.
  void log_gl_params() {
    GLenum params[] = {
      GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
      GL_MAX_CUBE_MAP_TEXTURE_SIZE,
      GL_MAX_DRAW_BUFFERS,
      GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
      GL_MAX_TEXTURE_IMAGE_UNITS,
      GL_MAX_TEXTURE_SIZE,
      GL_MAX_VERTEX_ATTRIBS,
      GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
      GL_MAX_VERTEX_UNIFORM_COMPONENTS,
      GL_MAX_VIEWPORT_DIMS,
      GL_STEREO,
    };

    const char* names[] = {
      "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
      "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
      "GL_MAX_DRAW_BUFFERS",
      "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
      "GL_MAX_TEXTURE_IMAGE_UNITS",
      "GL_MAX_TEXTURE_SIZE",
      "GL_MAX_VERTEX_ATTRIBS",
      "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
      "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
      "GL_MAX_VIEWPORT_DIMS",
      "GL_STEREO",
    };

    std::ostringstream ss;
    ss << "GL Context params: " << std::endl;
    for (int i = 0; i < 9; ++i) {
      int v = 0;
      glGetIntegerv(params[i], &v);
      ss << names[i] << ": " << v << std::endl;
    }

    // Others
    int v[2];
    v[0] = v[1] = 0;
    glGetIntegerv(params[9], v);
    ss << names[9] << ": " << v[0] << " " << v[1] << std::endl;
    unsigned char s = 0;
    glGetBooleanv(params[10], &s);
    ss << names[10] << ": " << static_cast<unsigned int>(s); 

    logging::write(GL_LOG_FILE, ss.str());
  }

  double s_previous_seconds;
  int s_frame_count;
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

  glfwSetWindowSizeCallback(window, glfw_window_size_callback);
  glfwSetFramebufferSizeCallback(window, glfw_framebuffer_resize_callback);

  glfwMakeContextCurrent(window);
  // Log the return but don't treat it as an error, this way we can attempt
  // to get the opengl info below on failure.
  ss << "gl3wInit() returned: " << gl3wInit() << std::endl;

  // Log the gl info.
  ss << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  ss << "OpenGL verion supported: " << glGetString(GL_VERSION);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  logging::write(GL_LOG_FILE, ss.str());
  log_gl_params();

  s_current_window = window;

  return window;
}

void gl::update_fps_counter(GLFWwindow* window) {
  double current_seconds;
  double elapsed_seconds;
  current_seconds = glfwGetTime();
  elapsed_seconds = current_seconds - s_previous_seconds;
  if (elapsed_seconds > 0.25) {
    s_previous_seconds = current_seconds;
    static char tmp[128];
    double fps = static_cast<double>(s_frame_count) / elapsed_seconds;
    sprintf(tmp, "opengl @ fps: %.2f", fps);
    glfwSetWindowTitle(window, tmp);
    s_frame_count = 0;
  }
  ++s_frame_count;
}

void gl::set_current_window(GLFWwindow* window) {
  s_current_window = window;
}

GLFWwindow* gl::get_current_window() {
  return s_current_window;
}
