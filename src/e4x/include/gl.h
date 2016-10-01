#pragma once

struct GLFWwindow;

namespace gl {
  // If fullscreen is enabled width and height are ignored.
  GLFWwindow* initialize(const char* title, bool fullscreen, int width=640, int height=480);
  void update_fps_counter(GLFWwindow* window);

  void set_current_window(GLFWwindow* window);
  GLFWwindow* get_current_window();
}
