#include "prop.h"

#include "mesh.h"
#include "program.h"
#include "camera.h"
#include "map.h"
#include "glm_hex.h"
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace prop {
  std::vector<glm::ivec3> s_spinagons;
  GLuint s_outlineshader;
  Mesh* s_tile;
  std::vector<GLint> s_outlineuniforms;

  glm::vec4 s_spinagoncolor = glm::vec4(0.48f, 0.6f, 0.98f, 0.55f);
}

void prop::initialize() {
  // Steal the existing mesh from map.
  s_tile = map::get_tile_mesh();
  s_outlineshader = program::get("outline"); 

  s_outlineuniforms.resize(4);
  s_outlineuniforms[0] = glGetUniformLocation(s_outlineshader, "view");
  s_outlineuniforms[1] = glGetUniformLocation(s_outlineshader, "proj");
  s_outlineuniforms[2] = glGetUniformLocation(s_outlineshader, "model");
  s_outlineuniforms[3] = glGetUniformLocation(s_outlineshader, "h_color");
}

void prop::spinagon(const glm::ivec3& cube) {
  s_spinagons.push_back(cube);
}

void prop::render() {
  static float rot = 0.0f;
  rot += 0.015f;

  for (const auto& loc : s_spinagons) {
    glm::vec2 world = glm_hex::cube_to_world(loc, 3);
    // Draw a fancy mesh.
    mesh::set_position(s_tile, glm::vec3(world.x, world.y, 0.1f));
    mesh::set_rotate(s_tile, rot, glm::vec3(0.0f, 0.0f, 1.0f));
    mesh::set_scale(s_tile, glm::vec3(0.5f, 0.5f, 0.5f));
    mesh::update_transform(s_tile);

    glUseProgram(s_outlineshader);

    Camera* c = camera::get_current();
    if (!c) return;

    glUniformMatrix4fv(s_outlineuniforms[0], 1, GL_FALSE, glm::value_ptr(c->m_view));
    glUniformMatrix4fv(s_outlineuniforms[1], 1, GL_FALSE, glm::value_ptr(c->m_projection));
    glUniformMatrix4fv(s_outlineuniforms[2], 1, GL_FALSE, glm::value_ptr(s_tile->m_matrix));
    glUniform4fv(s_outlineuniforms[3], 1, glm::value_ptr(s_spinagoncolor));

    glDisable(GL_DEPTH_TEST);
    mesh::raw_draw(s_tile);
    glEnable(GL_DEPTH_TEST);
    mesh::set_scale(s_tile, glm::vec3(1.0f, 1.0f, 1.0f));
    mesh::set_rotate(s_tile, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
  }

  s_spinagons.clear();
}

