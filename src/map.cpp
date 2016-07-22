#include "map.h"

#include "sim_interface.h"
#include "mesh.h"
#include "camera.h"
#include "geometry.h"
#include "glm_hex.h"
#include "gl.h"
#include "raycast.h"
#include "program.h"
#include "texloader.h"
#include "selection.h"

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <vector>
#include <iostream>

namespace map {
  glm::ivec3 s_selected;
  Mesh* s_mesh = nullptr;
  Mesh* s_pawnmesh = nullptr;
  Mesh* s_rookmesh = nullptr;
  Mesh* s_queenmesh = nullptr;
  GLint s_texloc = 0;
  glm::vec4 s_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

  void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    glm::vec3 from;
    glm::vec3 r = ray::from_mouseclick(xpos, ypos);

    Camera* c = camera::get_current();
    if (!c) return;
    float distance;
    // Intersect with the xy plane positioned at origin.
    if (!glm::intersectRayPlane(c->m_position, 
        r, 
        // Hexagon is height 0.3f, so lift the plane by that much.
        glm::vec3(0.0f, 0.0f, 0.0f), 
        glm::vec3(0.0f, 0.0f, 1.0f), 
        distance)) {
      return;
    }

    // The intersected point is the defined by p = camera_position + ray * distance.
    glm::vec3 position = c->m_position + r * distance;
    glm::ivec3 cube = glm_hex::world_to_cube(glm::vec2(position.x, position.y), 3);
    s_selected = cube;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      selection::lclick(s_selected);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
      selection::rclick(s_selected);
    }
  }

  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
      sim_interface::end_turn();
    }
    else if (key == GLFW_KEY_J && action == GLFW_PRESS) {
      sim_interface::join_player();
    }
    else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
      sim_interface::join_barbarian();
    }
  }

  void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Camera* c = camera::get_current();
    if (!c) return;
    c->zoom(static_cast<float>(yoffset));
  }
}

void map::initialize() {
  Camera* c = camera::get_current();

  std::vector<GLfloat> verts, norms;
  std::vector<GLuint> indcs;
  geometry::get_hexagon(verts, norms, indcs);

  GLuint tex;
  texloader::load("grass.png", GL_TEXTURE0, tex);
  texloader::load("water.png", GL_TEXTURE1, tex);
  texloader::load("desert.png", GL_TEXTURE2, tex);
  texloader::load("plains.png", GL_TEXTURE3, tex);
  texloader::load("mountain.png", GL_TEXTURE4, tex);

  s_mesh = mesh::create(std::move(verts), std::move(norms), std::move(indcs), {
    program::get("texture")
  });

  s_pawnmesh = mesh::create("pawn.obj", { program::get("phong") });
  s_rookmesh = mesh::create("rook.obj", { program::get("phong") });
  s_queenmesh = mesh::create("queen.obj", { program::get("phong") });

  mesh::bind_texture_data(s_mesh, geometry::get_hexagontexcoords());
  mesh::add_uniform(s_mesh, "basic_texture", &s_texloc);

  GLFWwindow* w = gl::get_current_window();

  if (w) {
    glfwSetMouseButtonCallback(w, mouse_button_callback);
    glfwSetScrollCallback(w, mouse_scroll_callback);
    glfwSetKeyCallback(w, key_callback);
  }
}

void map::teardown() {
  delete s_queenmesh;
  delete s_rookmesh;
  delete s_pawnmesh;
  delete s_mesh;
}

void map::update(double delta) {
  if (sim_interface::poll()) {
    sim_interface::synch();
  }
}

void map::draw() {
  glm::ivec3 pos;
  const world_map::TileMap& tiles = sim_interface::get_map();
  for (const auto& t : tiles) {
    pos = glm::ivec3(t.first.x, t.first.y, t.first.z);
    glm::vec2 world = glm_hex::cube_to_world(pos, 3);
    mesh::set_position(s_mesh, glm::vec3(world.x, world.y, 0.0f));
    s_texloc = 0;
    switch (t.second.m_terrain_type) {
    case TERRAIN_TYPE::DESERT:
      s_texloc = 2;
      break;
    case TERRAIN_TYPE::GRASSLAND:
      s_texloc = 0;
      break;
    case TERRAIN_TYPE::MOUNTAIN:
      s_texloc = 4;
      break;
    case TERRAIN_TYPE::PLAINS:
      s_texloc = 3;
      break;
    case TERRAIN_TYPE::WATER:
      s_texloc = 1;
      break;
    case TERRAIN_TYPE::UNKNOWN:
      break;
    }
    mesh::draw(s_mesh);
  }

  const std::vector<Unit>& units = sim_interface::get_units();
  for (const auto& u : units) {
    pos = glm::ivec3(u.m_location.x, u.m_location.y, u.m_location.z);
    glm::vec2 world = glm_hex::cube_to_world(pos, 3);
    Mesh* todraw;
    switch (u.m_unit_type) {
    case UNIT_TYPE::PHALANX:
      todraw = s_rookmesh;
      break;
    case UNIT_TYPE::SCOUT:
      todraw = s_pawnmesh;
    case UNIT_TYPE::WORKER:
      todraw = s_queenmesh;
      break;
    case UNIT_TYPE::ARCHER:
    case UNIT_TYPE::UNKNOWN:
      todraw = s_pawnmesh;
      break;
    }
    mesh::set_position(todraw, glm::vec3(world.x, world.y, 0.0f));
    mesh::draw(todraw);
  }
}
