#include "Mesh.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

Mesh::Mesh(const glm::vec3& start, 
    const std::vector<GLfloat>& vertices, 
    const std::vector<GLfloat>& colors) {
  m_position = start;
  m_vertices = vertices;
  m_colors = colors;
}

Mesh::~Mesh() {
}

// Binds vertex data to GL.
void Mesh::initialize() {
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

  // TEMP
  // TODO: Shader should return cached compiled file.
  GLuint vs = shader::compile_from_file(GL_VERTEX_SHADER, "simple_transform.vert");
  GLuint fs = shader::compile_from_file(GL_FRAGMENT_SHADER, "simple.frag");
  m_shaders.push_back(vs);   
  m_shaders.push_back(fs);
  m_program = shader::link({ vs, fs });

  m_mat_uniform = glGetUniformLocation(m_program, "matrix");
}

void Mesh::update(float delta) {
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

