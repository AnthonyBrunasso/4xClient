#pragma once

#include <GL/gl3w.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <functional>
#include <vector>
#include <utility>
#include <functional>

class Mesh {
public:
  Mesh(const glm::vec3& position, 
      const std::vector<GLfloat>& vertices, 
      const std::vector<std::pair<GLenum, std::string> >& shaders);

  Mesh(const glm::vec3& position, 
      const std::vector<GLfloat>& vertices, 
      const std::vector<GLfloat>& colors,
      const std::vector<std::pair<GLenum, std::string> >& shaders);

  ~Mesh();

  void initialize_shaders();
  // Deletes and recompiles shaders.
  void reset_shaders();

  // Binds vertex data to GL.
  virtual void initialize();

  // Update called per frame.
  virtual void update(float delta);

  virtual void draw();

  void add_predraw(std::function<void(GLuint)> op);
  void add_preupdate(std::function<void(float, glm::vec3&)> op);

  void set_position(const glm::vec3& position) { m_position = position; };

private:
  std::vector<GLfloat> m_vertices;
  std::vector<GLfloat> m_colors;

  std::vector<GLuint> m_shader_ids;
  GLuint m_program;

  // Do I need the vbo ints in here?
  GLuint m_points_vbo;
  GLuint m_color_vbo;
  GLuint m_vao;
  glm::vec3 m_position;

  GLint m_mat_uniform;
  glm::mat4 m_matrix;
  std::vector<std::pair<GLenum, std::string> > m_shaders;
  std::vector<std::function<void(GLuint)> > m_predraw_ops;
  std::vector<std::function<void(float, glm::vec3&)> > m_preupdate_ops;
};
