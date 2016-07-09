#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "gl.h"

Camera::Camera(float near_plane, float far_plane, float fov, float aspect) :
    m_speed(10.0f) {
  m_projection = glm::perspective(fov, aspect, near_plane, far_plane);
  m_position = glm::vec3(0.0f, 0.0f, -20.0f);
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

void camera::set_uniforms(GLuint program, Camera* camera) {
  if (!camera) return;

  // Shader view and projection matrices are always named view/proj.
  GLint view_mat_location = glGetUniformLocation(program, "view");
  glUniformMatrix4fv(view_mat_location, 
    1, 
    GL_FALSE, 
    glm::value_ptr(camera->m_view));

  GLint proj_mat_location = glGetUniformLocation(program, "proj");
  glUniformMatrix4fv(proj_mat_location, 
    1, 
    GL_FALSE, 
    glm::value_ptr(camera->m_projection));
}
