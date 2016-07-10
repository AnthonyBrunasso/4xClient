#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


#include "gl.h"

namespace camera {
  // This pointer will point to the most recently created camera
  Camera* s_camera = nullptr;
}

Camera::Camera(float near_plane, float far_plane, float fov, float aspect) :
    m_speed(10.0f) {
  m_projection = glm::perspective(fov, aspect, near_plane, far_plane);
  m_position = glm::vec3(0.0f, -6.0f, 8.0f);
  m_forward = glm::vec3(0.0f, 0.20f, -0.15f);
  m_up = glm::vec3(0.0f, 1.0f, 0.0f);
  camera::s_camera = this;
}

void Camera::update(float delta) {
  GLFWwindow* window = gl::get_current_window();   

  if (window) {
    float c_speed = m_speed * delta;

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
}


void camera::set_uniforms(GLuint program, GLint view_loc, GLint proj_loc, Camera* c) {
  if (!c) return;

  // Shader view and projection matrices are always named view/proj.
  glUniformMatrix4fv(view_loc,
    1, 
    GL_FALSE, 
    glm::value_ptr(c->m_view));

  glUniformMatrix4fv(proj_loc,
    1, 
    GL_FALSE, 
    glm::value_ptr(c->m_projection));
}

Camera* camera::get_current() {
  return s_camera;
}
