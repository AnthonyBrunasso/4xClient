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
  glm::vec3 m_position;
  glm::vec3 m_scale;
  glm::vec3 m_rotate_axis;
  float m_degree;

  // All the programs and uniforms for this mesh
  std::vector<std::pair<GLuint, std::vector<GLint> > > m_programs;
  // Starting from index 3 and on, from the vector above, this is 
  // the uniform data that will be bound.
  std::vector<GLfloat*> m_uniform_data;

  std::vector<GLfloat> m_vertices;
  std::vector<GLuint> m_indices;
  std::vector<GLfloat> m_normals;

  std::vector<GLuint> m_shader_ids;

  std::vector<GLuint> m_vbos;

  GLuint m_vao;

  bool m_update_matrix;
  glm::mat4 m_matrix;
};

namespace mesh {
  // If uniforms exist for MVP they will exist
  // at these locations in the m_uniforms array.
  static const uint32_t VIEW_IDX = 0; 
  static const uint32_t PROJ_IDX = 1; 
  static const uint32_t MODL_IDX = 2; 

  Mesh* create(const std::string& filename, const std::vector<GLuint>& programs);

  Mesh* create(const std::vector<GLfloat>& vertices
    , const std::vector<GLfloat>& normals
    , const std::vector<GLuint>& indices
    , const std::vector<GLuint>& programs);

  void draw(Mesh* m);

  // Set property of mesh and update its transform matrix.
  void set_position(Mesh* m, const glm::vec3& position);
  void set_scale(Mesh* m, const glm::vec3& scale);
  void set_rotate(Mesh* m, float degree, const glm::vec3& axis);

  void update_transform(Mesh* m);
  void add_uniform(Mesh* m, const char* name, GLfloat *value);
}
