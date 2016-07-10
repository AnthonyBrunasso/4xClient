#include "map.h"

#include "sim_interface.h"
#include "mesh.h"
#include "camera.h"
#include "geometry.h"
#include "glm_hex.h"

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace map {
  world_map::TileMap s_tiles;
  UColorMesh s_mesh{glm::vec3(0.0f, 0.0f, 0.0f), geometry::get_hexagon(), {
    {GL_VERTEX_SHADER, "simple_perspective.vert"}, 
    {GL_FRAGMENT_SHADER, "simple_uniform_color.frag"}
  }};
}

void map::initialize() {
  Camera* c = camera::get_current();

  s_mesh.initialize();
  s_tiles = sim_interface::get_map();

  // Setup mesh to use perspective and camera view.
  auto set_view = [c](GLuint program) {
    camera::set_uniforms(program, s_mesh.m_view_mat_loc, s_mesh.m_proj_mat_loc, c);
  };

  s_mesh.add_predraw(set_view);
}

void map::update() {
}

void map::draw() {
  glm::vec3 pos;
  for (const auto& t : s_tiles) {
    pos = glm::vec3(t.first.x, t.first.y, t.first.z);
    glm::vec2 world = glm_hex::cube_to_world(pos, 1);
    s_mesh.set_position(glm::vec3(world.x, world.y, 0.0f));
    switch (t.second.m_terrain_type) {
    case TERRAIN_TYPE::DESERT:
      s_mesh.set_color(glm::vec4(0.97f, 0.80f, 0.70f, 1.0f));
      break;
    case TERRAIN_TYPE::GRASSLAND:
      s_mesh.set_color(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
      break;
    case TERRAIN_TYPE::MOUNTAIN:
      s_mesh.set_color(glm::vec4(0.51f, 0.51f, 0.51f, 1.0f));
      break;
    case TERRAIN_TYPE::PLAINS:
      s_mesh.set_color(glm::vec4(0.62f, 0.32f, 0.17f, 1.0f));
      break;
    case TERRAIN_TYPE::WATER:
      s_mesh.set_color(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
      break;
    case TERRAIN_TYPE::UNKNOWN:
      break;
    }
    s_mesh.draw();
  }
}
