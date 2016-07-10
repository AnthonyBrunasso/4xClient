#include "Mesh.h"

#include <sstream>

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "tiny_obj_loader.h"
#include "log.h"

#define MESH_LOG_FILE "mesh.log"
#define MESH_DIR "../../data/models"

namespace mesh {
  void load_mesh(const std::string& filename, Mesh& mesh) {
    std::string file = MESH_DIR;
    file += "/" + filename;

    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    if (!tinyobj::LoadObj(shapes, materials, err, file.c_str())) {
      logging::write(MESH_LOG_FILE, err);
      return;
    }

    std::ostringstream ss;
    for (size_t i = 0; i < shapes.size(); ++i) {
      ss << "Loading - " << (shapes[i].name.empty() ? "Unknown name" : shapes[i].name) << std::endl;
      ss << "  size: " << shapes[i].mesh.indices.size() << std::endl;
      ss << "  material ids: " << shapes[i].mesh.material_ids.size() << std::endl;
      for (size_t j = 0, e = shapes[i].mesh.positions.size(); j < e; ++j) {
        mesh.m_vertices.push_back(shapes[i].mesh.positions[j]);
      }

      for (size_t j = 0, e = shapes[i].mesh.indices.size(); j < e; ++j) {
        mesh.m_indices.push_back(shapes[i].mesh.indices[j]);
      }
    }

    logging::write(MESH_LOG_FILE, ss.str());
  }
}

Mesh::Mesh(const glm::vec3& position, 
    const std::string& filename, 
    const std::vector<std::pair<GLenum, std::string> >& shaders) :
    m_position(position)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_rotate_axis(0.0f, 0.0f, 1.0f)
    , m_degree(0.0f)
    , m_program(0)
    , m_shaders(shaders) {
  mesh::load_mesh(filename, *this);
}


Mesh::Mesh(const glm::vec3& position, 
      const std::vector<GLfloat>& vertices, 
      const std::vector<std::pair<GLenum, std::string> >& shaders) : 
  m_position(position)
  , m_scale(1.0f, 1.0f, 1.0f)
  , m_rotate_axis(0.0f, 0.0f, 1.0f)
  , m_degree(0.0f)
  , m_vertices(vertices)
  , m_indices()
  , m_colors()
  , m_program(0) 
  , m_shaders(shaders) {
}


Mesh::Mesh(const glm::vec3& position,
    const std::vector<GLfloat>& vertices, 
    const std::vector<GLfloat>& colors,
    const std::vector<std::pair<GLenum, std::string> >& shaders) : 
  m_position(position)
  , m_scale(1.0f, 1.0f, 1.0f)
  , m_rotate_axis(0.0f, 0.0f, 1.0f)
  , m_degree(0.0f)
  , m_vertices(vertices)
  , m_indices()
  , m_colors(colors)
  , m_program(0) 
  , m_shaders(shaders) {
}

Mesh::~Mesh() {
}

void Mesh::initialize_shaders() {
  for (auto s : m_shaders) {
    GLuint shader = shader::compile_from_file(s.first, s.second.c_str());
    m_shader_ids.push_back(shader);
  }
  m_program = shader::link(m_shader_ids);

  // Setup the uniform locations.
  get_uniform_locations();
}

void Mesh::reset_shaders() {
  // First delete the meshes program
  glDeleteProgram(m_program);
  // Delete its shaders
  for (auto id : m_shader_ids) {
    glDeleteShader(id); 
  }
  // Clear the shader ids, they are invalid now.
  m_shader_ids.clear();
  // Rebuild the shaders and relink the program.
  initialize_shaders();
}

// Binds vertex data to GL.
void Mesh::initialize() {
  if (m_program == 0) {
    initialize_shaders();
  }

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  if (m_vertices.size()) {
    m_points_vbo = 0;
    glGenBuffers(1, &m_points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_points_vbo);
    glBufferData(GL_ARRAY_BUFFER, 
      m_vertices.size() * sizeof(GLfloat), 
      m_vertices.data(), 
      GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  if (m_colors.size()) {
    m_color_vbo = 0;
    glGenBuffers(1, &m_color_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
    glBufferData(GL_ARRAY_BUFFER,
      m_colors.size() * sizeof(GLfloat),
      m_colors.data(),
      GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  if (m_indices.size()) {
    m_index_vbo = 0;
    glGenBuffers(1, &m_index_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
      m_indices.size() * sizeof(GLuint),
      m_indices.data(),
      GL_STATIC_DRAW);
  }

  // The VAO cannot be changed outside of this function.
  glBindVertexArray(0);
}

void Mesh::update(float delta) {
  for (auto op : m_preupdate_ops) {
    op(delta, m_position); 
  }
}

void Mesh::draw() {
  glUseProgram(m_program);
  if (m_model_mat_loc != -1) {
    // Move it to the meshes position.

    m_matrix = glm::translate(glm::mat4(), m_position);
    m_matrix = glm::rotate(m_matrix, m_degree, m_rotate_axis);
    m_matrix = glm::scale(m_matrix, m_scale);

    glUniformMatrix4fv(m_model_mat_loc, 1, GL_FALSE, glm::value_ptr(m_matrix));
  }

  for (auto op : m_predraw_ops) {
    op(m_program);
  }

  glBindVertexArray(m_vao);

  if (m_indices.size()) {
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
  }
  else {
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 3);
  }

  glBindVertexArray(0);
}

void Mesh::add_predraw(std::function<void(GLuint)> op) {
  m_predraw_ops.push_back(op);
}

void Mesh::add_preupdate(std::function<void(float, glm::vec3&)> op) {
  m_preupdate_ops.push_back(op);
}

void Mesh::get_uniform_locations() {
  // MVP should be apart of every shader pipeline/mesh.
  m_view_mat_loc = glGetUniformLocation(m_program, "view");
  m_proj_mat_loc = glGetUniformLocation(m_program, "proj");
  m_model_mat_loc = glGetUniformLocation(m_program, "model");
}

void UColorMesh::get_uniform_locations() {
  Mesh::get_uniform_locations();
  m_color_loc = glGetUniformLocation(m_program, "color");
  // Set the color before drawing.
  auto predraw = [&](GLuint program) {
    glUniform4fv(m_color_loc,
      1,
      glm::value_ptr(m_color));
  };
  add_predraw(predraw);
}
