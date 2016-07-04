#pragma once

struct GLFWwindow;

namespace gl {
  GLFWwindow* initialize(int width, int height, const char* title);
}
