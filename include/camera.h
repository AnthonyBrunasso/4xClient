#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera {
public:
  Camera(float near, float far, float fov, float aspect);

  void update(float delta);

  float m_speed;

  glm::vec3 m_position;
  glm::mat4 m_view;
  glm::mat4 m_projection;
};
