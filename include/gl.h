#pragma once

struct GLFWwindow;

namespace gl {
  // If fullscreen is enabled width and height are ignored.
  GLFWwindow* initialize(const char* title, bool fullscreen, int width=640, int height=480);
}
