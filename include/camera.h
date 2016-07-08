#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera {
public:
  Camera(float near, float far, float fov, float aspect);

  void update();
  const glm::mat4& mat() const;

  float m_speed;
  float m_yaw_speed;

  glm::vec3 m_position;
  float m_yaw;

  glm::mat4 m_view;
  glm::mat4 m_projection;
};
