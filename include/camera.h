#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <glm/gtc/quaternion.hpp>

class Camera {
public:
  Camera(float near_plane, float far_plane, float fov, float aspect);

  void update(float delta);

  glm::mat4 m_view;
  glm::mat4 m_projection;

  glm::vec3 m_position;
  glm::vec3 m_forward;
  glm::vec3 m_up;

  float m_speed;
  float m_xpos;
  float m_ypos;

  float m_yaw;
  float m_pitch;
};

namespace camera {
  void set_uniforms(GLuint program, GLint view_loc, GLint proj_loc, Camera* c);
}
