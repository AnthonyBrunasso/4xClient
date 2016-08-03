#include "raycast.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/vec4.hpp>
#include <glm/glm.hpp>

#include "gl.h"
#include "camera.h"

glm::vec3 ray::from_mouse(double mx, double my) {
  // Require the current window for frame buffer dimensions.
  GLFWwindow* window = gl::get_current_window();
  if (!window) return glm::vec3(0.0f, 0.0f, 0.0f);
  // Require the camera for view matrix.
  Camera* camera = camera::get_current();
  if (!camera) return glm::vec3(0.0f, 0.0f, 0.0f);

  int width, height;
  glfwGetWindowSize(window, &width, &height);
  
  // Transform click into normalized device coordinates.
  // These should range: [-1:1, -1:1, -1:1]
  float x = (2.0f * static_cast<float>(mx)) / width - 1.0f;
  float y = 1.0f - (2.0f * static_cast<float>(my)) / height;
  float z = 1.0f;
  glm::vec3 ray_nds(x, y, z);

  // Transform to homogeneous clip coordinates
  glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);

  // Go from clip space to eye space. To get from eye space to clip space
  // a vector is multiplied by the projection matrix. 
  // P * v = c
  // To get v we can multiply c by the inverse matrix. 
  // v = P^(-1) * c
  glm::vec4 ray_eye = glm::inverse(camera->m_projection) * ray_clip;
  // Set as a vector and not a point.
  ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

  // Transform to world coordinates
  glm::vec4 ray_world4 = glm::inverse(camera->m_view) * ray_eye;
  glm::vec3 ray_world(ray_world4.x, ray_world4.y, ray_world4.z);
  return glm::normalize(ray_world);
}
