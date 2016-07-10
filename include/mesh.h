#pragma once

#include <GL/gl3w.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <functional>
#include <vector>
#include <utility>
#include <functional>

#include <cstdint>

class Mesh {
public:
  // This mesh loads a model.
  Mesh(const glm::vec3& position, 
      const std::string& filename, 
      const std::vector<std::pair<GLenum, std::string> >& shaders);


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
  void set_scale(const glm::vec3& scale) { m_scale = scale; };
  void set_rotate(float degree, const glm::vec3& axis) { m_degree = degree;  m_rotate_axis = axis; };
  virtual void get_uniform_locations();

  glm::vec3 m_position;
  glm::vec3 m_scale;
  glm::vec3 m_rotate_axis;
  float m_degree;

  GLint m_view_mat_loc;
  GLint m_proj_mat_loc;
  GLint m_model_mat_loc;

  std::vector<GLfloat> m_vertices;
  std::vector<GLuint> m_indices;
  std::vector<GLfloat> m_colors;

  std::vector<GLuint> m_shader_ids;
  GLuint m_program;

  // Do I need the vbo ints in here?
  GLuint m_points_vbo;
  GLuint m_index_vbo;
  GLuint m_color_vbo;
  GLuint m_vao;

  //GLint m_mat_uniform;
  glm::mat4 m_matrix;
  std::vector<std::pair<GLenum, std::string> > m_shaders;
  std::vector<std::function<void(GLuint)> > m_predraw_ops;
  std::vector<std::function<void(float, glm::vec3&)> > m_preupdate_ops;
};

// Mesh with a uniform color.
class UColorMesh : public Mesh {
public:
  UColorMesh(const glm::vec3& position, 
      const std::vector<GLfloat>& vertices, 
      const std::vector<std::pair<GLenum, std::string> >& shaders) :
        Mesh(position, vertices, shaders) {};

  void get_uniform_locations() override;
  void set_color(const glm::vec4& color) { m_color = color; };

  glm::vec4 m_color;
  GLint m_color_loc;
};
