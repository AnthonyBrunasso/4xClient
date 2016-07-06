#include "Mesh.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

Mesh::Mesh(const glm::vec3& start, 
    const std::vector<GLfloat>& vertices, 
    const std::vector<GLfloat>& colors,
    const std::vector<std::pair<GLenum, std::string> >& shaders) : 
  m_position(start)
  , m_vertices(vertices)
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

  GLuint m_points_vbo = 0;
  glGenBuffers(1, &m_points_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_points_vbo);
  glBufferData(GL_ARRAY_BUFFER, 
    m_vertices.size() * sizeof(GLfloat), 
    m_vertices.data(), 
    GL_STATIC_DRAW);

  GLuint m_colors_vbo = 0;
  glGenBuffers(1, &m_colors_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_colors_vbo);
  glBufferData(GL_ARRAY_BUFFER, 
    m_colors.size() * sizeof(GLfloat), 
    m_colors.data(), 
    GL_STATIC_DRAW);

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, m_points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, m_colors_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  m_mat_uniform = glGetUniformLocation(m_program, "matrix");
}

void Mesh::update(float delta) {
  static float speed = 1.0f;
  if (fabs(m_position.x) > 1.0f) {
    speed *= -1;
  }

  m_position.x += delta * speed;
}

void Mesh::draw() {
  glUseProgram(m_program);
  if (m_mat_uniform != -1) {
    // Set identity.
    m_matrix = glm::mat4();
    // Move it to the meshes position.
    m_matrix = glm::translate(m_matrix, m_position);
    glUniformMatrix4fv(m_mat_uniform, 1, GL_FALSE, glm::value_ptr(m_matrix));
  }

  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 3);
}

