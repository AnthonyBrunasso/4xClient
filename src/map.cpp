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
  std::vector<glm::ivec3> s_tiles;
  Camera* s_camera = nullptr;
  Mesh s_mesh{glm::vec3(0.0f, 0.0f, 0.0f), geometry::get_hexagon(), {
    {GL_VERTEX_SHADER, "simple_perspective.vert"}, 
    {GL_FRAGMENT_SHADER, "simple.frag"}
  }};
}

void map::initialize(Camera* c) {
  s_mesh.initialize();
  world_map::TileMap& m = sim_interface::get_map();
  for (const auto& t : m) {
    s_tiles.push_back(glm::ivec3(t.first.x, t.first.y, t.first.x));
  }

  s_camera = c;

  // Setup mesh to use perspective and camera view.
  auto set_view = [](GLuint program) {
    camera::set_uniforms(program, s_camera);
  };

  s_mesh.add_predraw(set_view);
}

void map::update() {
}

void map::draw() {
  for (const auto& cube_coord : s_tiles) {
    glm::vec2 world = glm_hex::cube_to_world(cube_coord, 1);
    s_mesh.set_position(glm::vec3(world.x, world.y, 0.0f));
    s_mesh.draw();
  }
}
