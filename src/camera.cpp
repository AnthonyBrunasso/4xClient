#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float near, float far, float fov, float aspect) :
    m_speed(1.0f) {
  //m_projection = glm::perspective(45.0f, 1024.0f/768.0f, 0.1f, 200.0f);
  m_projection = glm::perspective(fov, aspect, near, far);
  m_position = glm::vec3(0.0f, 0.0f, -10.0f);
}

void Camera::update() {
  m_view = glm::lookAt(m_position, 
      glm::vec3(0.0f, 0.0f, 0.0f), 
      glm::vec3(0.0f, 1.0f, 0.0f));
}
