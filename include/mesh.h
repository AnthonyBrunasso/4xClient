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
  // Starting from index 3 and on, from the vector above in the pair, this is 
  // the uniform data that will be bound. The first three are for the model matrix.
  std::vector<std::pair<GLenum, GLfloat*> > m_uniform_fdata;
  std::vector<GLint*> m_uniform_idata;
  std::vector<GLuint> m_vbos;

  std::vector<GLfloat> m_vertices;
  std::vector<GLuint> m_indices;
  std::vector<GLfloat> m_normals;

  // If texcoords is empty the mesh does not use a texture.
  std::vector<GLfloat> m_texcoords;

  GLuint m_vao;

  bool m_update_matrix;
  glm::mat4 m_matrix;
 
  // Order is ka, kd, ks. 
  std::vector<glm::vec3> m_light_material;
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

  // Textures are not initialized on creation. Bind it seperately.
  void bind_texture_data(Mesh* m, const std::vector<GLfloat>& texcoords);

  // Set property of mesh and update its transform matrix.
  void set_position(Mesh* m, const glm::vec3& position);
  void set_scale(Mesh* m, const glm::vec3& scale);
  void set_rotate(Mesh* m, float degree, const glm::vec3& axis);

  void update_transform(Mesh* m);
  // Add all floating uniforms before integer uniforms. 
  void add_uniform(Mesh* m, const char* name, GLenum type, GLfloat *value);
  void add_uniform(Mesh* m, const char* name, GLint *value);

  // Get all the number of vertices that has been sent to the gpu, but not necessarily rendering. 
  uint32_t get_vert_count();
}
