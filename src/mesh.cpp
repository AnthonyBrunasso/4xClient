#include "mesh.h"

#include <sstream>

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "shader.h"
#include "tiny_obj_loader.h"
#include "log.h"
#include "camera.h"

#define MESH_LOG_FILE "mesh.log"
#define MESH_DIR "../../data/models"

namespace mesh {
  static const uint32_t VERT_VBO_IDX = 0; 
  static const uint32_t NORM_VBO_IDX = 1; 
  static const uint32_t INDX_VBO_IDX = 2; 

  void load_from_file(Mesh* m, const std::string& filename) {
    if (!m) return;
    std::string file = MESH_DIR;
    file += "/" + filename;

    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    if (!tinyobj::LoadObj(shapes, materials, err, file.c_str(), NULL, tinyobj::calculate_normals)) {
      logging::write(MESH_LOG_FILE, err);
      return;
    }

    std::ostringstream ss;
    for (size_t i = 0; i < shapes.size(); ++i) {
      ss << "Loading - " << (shapes[i].name.empty() ? "Unknown name" : shapes[i].name) << std::endl;
      ss << "  size: " << shapes[i].mesh.indices.size() << std::endl;
      ss << "  material ids: " << shapes[i].mesh.material_ids.size() << std::endl;
      m->m_vertices = std::move(shapes[i].mesh.positions);
      m->m_indices = std::move(shapes[i].mesh.indices);
      m->m_normals = std::move(shapes[i].mesh.normals);
    }

    logging::write(MESH_LOG_FILE, ss.str());
  }

  // Binds vertex, index and normal data to the vao.
  void bind_vertex_data(Mesh* m) {
    if (!m) return;
    glGenVertexArrays(1, &m->m_vao);
    glBindVertexArray(m->m_vao);

    m->m_vbos.resize(3);
    glGenBuffers(3, m->m_vbos.data());

    if (m->m_vertices.size()) {
      glBindBuffer(GL_ARRAY_BUFFER, m->m_vbos[VERT_VBO_IDX]);
      glBufferData(GL_ARRAY_BUFFER, 
        m->m_vertices.size() * sizeof(GLfloat), 
        m->m_vertices.data(), 
        GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    if (m->m_normals.size()) {
      glBindBuffer(GL_ARRAY_BUFFER, m->m_vbos[NORM_VBO_IDX]);
      glBufferData(GL_ARRAY_BUFFER,
        m->m_normals.size() * sizeof(GLfloat),
        m->m_normals.data(),
        GL_STATIC_DRAW);

      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    if (m->m_indices.size()) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->m_vbos[INDX_VBO_IDX]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        m->m_indices.size() * sizeof(GLuint),
        m->m_indices.data(),
        GL_STATIC_DRAW);
    }

    // The VAO cannot be changed outside of this function unless explicitly rebound.
    glBindVertexArray(0);
  }

  // Meshes always have mvps.
  void setup_mvp(Mesh* m) {
    if (!m) return;
    // Do it for all the programs on the current mesh.
    for (auto& p : m->m_programs) {
      GLint view = glGetUniformLocation(p.first, "view");
      GLint proj = glGetUniformLocation(p.first, "proj");
      GLint modl = glGetUniformLocation(p.first, "model");

      // These uniforms must exist in all vertex shaders.
      assert(view != -1);
      assert(proj != -1);
      assert(modl != -1);

      p.second.resize(3);
      p.second[VIEW_IDX] = view;
      p.second[PROJ_IDX] = proj;
      p.second[MODL_IDX] = modl;
    }
  }

  void set_programs(Mesh* m, const std::vector<GLuint>& programs) {
    if (!m) return;
    for (auto& p : programs) {
      m->m_programs.push_back(std::pair<GLuint, std::vector<GLint> >(p, {}));
    }
  }
}

Mesh* mesh::create(const std::string& filename, const std::vector<GLuint>& programs) {
  Mesh* mesh = new Mesh();
  mesh::set_position(mesh, glm::vec3(0.0f, 0.0f, 0.0f));
  mesh::set_rotate(mesh, 0, glm::vec3(0.0f, 1.0f, 0.0f));
  mesh::set_scale(mesh, glm::vec3(1.0f, 1.0f, 1.0f));
  set_programs(mesh, programs);
  load_from_file(mesh, filename);
  bind_vertex_data(mesh);
  setup_mvp(mesh);
  return mesh;
}

Mesh* mesh::create(const std::vector<GLfloat>& vertices
    , const std::vector<GLfloat>& normals
    , const std::vector<GLuint>& indices
    , const std::vector<GLuint>& programs) {
  Mesh* mesh = new Mesh();
  mesh::set_position(mesh, glm::vec3(0.0f, 0.0f, 0.0f));
  mesh::set_rotate(mesh, 0, glm::vec3(0.0f, 1.0f, 0.0f));
  mesh::set_scale(mesh, glm::vec3(1.0f, 1.0f, 1.0f));
  mesh->m_vertices = vertices;
  mesh->m_normals = normals;
  mesh->m_indices = indices;
  mesh->m_update_matrix = true;
  set_programs(mesh, programs);
  bind_vertex_data(mesh);
  setup_mvp(mesh);
  return mesh;
}

void mesh::draw(Mesh* m) {
  if (!m) return;
  // Update the transform.
  for (const auto& p : m->m_programs) {
    glUseProgram(p.first);
    mesh::update_transform(m);

    const std::vector<GLint>& uniforms = p.second;
    // At the very least the mvp must exist for all meshes.
    uint32_t ucount = 3;

    // All meshes must have mvp uniforms.
    assert(uniforms.size() >= ucount); 

    Camera* c = camera::get_current();

    // Bind view, proj and model.
    if (c) {
      // Bind the view and projection if the camera is valid.
      glUniformMatrix4fv(uniforms[VIEW_IDX], 1, GL_FALSE, glm::value_ptr(c->m_view));
      glUniformMatrix4fv(uniforms[PROJ_IDX], 1, GL_FALSE, glm::value_ptr(c->m_projection));
    }
    else {
      // Identity otherwise.
      glUniformMatrix4fv(uniforms[VIEW_IDX], 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
      glUniformMatrix4fv(uniforms[PROJ_IDX], 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    }

    glUniformMatrix4fv(uniforms[MODL_IDX], 1, GL_FALSE, glm::value_ptr(m->m_matrix));

    // Bind any other uniforms the mesh may have.
    uint32_t idx = 0;
    for (uint32_t i = ucount, e = ucount + m->m_uniform_fdata.size(); i < e; ++i, ++idx) {
      glUniform4fv(uniforms[i], 1, m->m_uniform_fdata[idx]);
      ++ucount;
    }

    idx = 0;
    for (uint32_t i = ucount, e = ucount + m->m_uniform_idata.size(); i < e; ++i, ++idx) {
      glUniform1iv(uniforms[i], 1, m->m_uniform_idata[idx]);
      ++ucount;
    }

    glBindVertexArray(m->m_vao);
    // Use indices if the exist.
    if (m->m_indices.size()) {
      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m->m_indices.size()), GL_UNSIGNED_INT, 0);
    }
    // Otherwise draw the verts as triangles.
    else {
      glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m->m_vertices.size()) / 3);
    }
    glBindVertexArray(0);
  }
}

void mesh::bind_texture_data(Mesh* m, const std::vector<GLfloat>& texcoords) {
  if (!m) return;
  glBindVertexArray(m->m_vao);

  m->m_texcoords = texcoords;
  m->m_vbos.push_back(0);
  GLuint& vbo = m->m_vbos.back();
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER,
    m->m_texcoords.size() * sizeof(GLfloat),
    m->m_texcoords.data(),
    GL_STATIC_DRAW);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

// Set property of mesh and update its transform matrix.
void mesh::set_position(Mesh* m, const glm::vec3& position) {
  if (!m) return;
  m->m_position = position;
  m->m_update_matrix = true;
}

void mesh::set_scale(Mesh* m, const glm::vec3& scale) {
  if (!m) return;
  m->m_scale = scale;
  m->m_update_matrix = true;
}

void mesh::set_rotate(Mesh* m, float degree, const glm::vec3& axis) {
  if (!m) return;
  m->m_degree = degree;
  m->m_rotate_axis = axis;
  m->m_update_matrix = true;
}

void mesh::update_transform(Mesh* m) {
  if (!m) return;
  // Rotations are expensive, don't do them if they can be avoided.
  if (!m->m_update_matrix) return;
  m->m_matrix = glm::translate(glm::mat4(), m->m_position);
  m->m_matrix = glm::rotate(m->m_matrix, m->m_degree, m->m_rotate_axis);
  m->m_matrix = glm::scale(m->m_matrix, m->m_scale);
  m->m_update_matrix = false;
}

// TODO: Make to support all different types of uniforms.
void mesh::add_uniform(Mesh* m, const char* name, GLfloat* value) {
  if (!m) return;
  // Do it for all the programs on the current mesh.
  for (auto& p : m->m_programs) {
    GLint loc = glGetUniformLocation(p.first, name);
    if (loc != -1) {
      p.second.push_back(loc);
      m->m_uniform_fdata.push_back(value);
    }
  }
}

void mesh::add_uniform(Mesh* m, const char* name, GLint* value) {
  if (!m) return;
  // Do it for all the programs on the current mesh.
  for (auto& p : m->m_programs) {
    GLint loc = glGetUniformLocation(p.first, name);
    if (loc != -1) {
      p.second.push_back(loc);
      m->m_uniform_idata.push_back(value);
    }
  }
}
