#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


#include "gl.h"

namespace e4x {

namespace camera {
  // This pointer will point to the most recently created camera
  Camera* s_camera = nullptr;
}

Camera::Camera(float near_plane, float far_plane, float fov, float aspect) :
  m_speed(25.0f) {
  m_projection = glm::perspective(fov, aspect, near_plane, far_plane);
  m_position = glm::vec3(0.0f, -16.0f, 28.0f);
  m_forward = glm::vec3(0.0f, 0.1f, -0.15f);
  m_up = glm::vec3(0.0f, 1.0f, 0.0f);
  camera::s_camera = this;
}

void Camera::update(double delta) {
  GLFWwindow* window = gl::get_current_window();

  if (window) {
    float c_speed = m_speed * static_cast<float>(delta);

    // Move camera with WASD
    if (glfwGetKey(window, GLFW_KEY_W)) {
      m_position.y += c_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_S)) {
      m_position.y -= c_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_A)) {
      m_position.x -= c_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_D)) {
      m_position.x += c_speed;
    }

    m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
  }

  m_delta = delta;
}

void Camera::zoom(float amount) {
  static const float sensitivity = 11.0f;
  float c_speed = m_speed * m_delta * amount * sensitivity;
  m_position += m_forward * c_speed;
}

Camera* camera::get_current() {
  return s_camera;
}

}
