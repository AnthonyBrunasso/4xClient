#include "camera.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "gl.h"

Camera::Camera(float near, float far, float fov, float aspect) :
    m_speed(10.0f) {
  //m_projection = glm::perspective(45.0f, 1024.0f/768.0f, 0.1f, 200.0f);
  m_projection = glm::perspective(fov, aspect, near, far);
  m_position = glm::vec3(0.0f, 0.0f, -3.0f);
}

void Camera::update(float delta) {
  GLFWwindow* window = gl::get_current_window();   

  if (window) {
    if (glfwGetKey(window, GLFW_KEY_LEFT)) {
      m_position.x += m_speed * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
      m_position.x -= m_speed * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_UP)) {
      m_position.y += m_speed * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN)) {
      m_position.y -= m_speed * delta;
    }
  }

  m_view = glm::lookAt(m_position, 
      glm::vec3(0.0f, 0.0f, 0.0f), 
      glm::vec3(0.0f, 1.0f, 0.0f));
}
