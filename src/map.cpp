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
#include "light.h"

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <vector>
#include <iostream>

namespace map {
  glm::ivec3 s_selected;
  Mesh* s_tile = nullptr;
  Mesh* s_pawnmesh = nullptr;
  Mesh* s_rookmesh = nullptr;
  Mesh* s_queenmesh = nullptr;
  Mesh* s_bishopmesh = nullptr;
  Mesh* s_hutmesh = nullptr;
  GLint s_texloc = 0;
  GLint s_usetex = 1;
  GLint s_dontusetex = 0;
  glm::vec4 s_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

  GLuint s_outlineshader;
  std::vector<GLint> s_outlineuniforms;

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
    else if (key == GLFW_KEY_I && action == GLFW_PRESS) {
      sim_interface::initial_settle();
    }
    else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
      sim_interface::settle();
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

  program::build("outline", {
    {GL_VERTEX_SHADER, "simple_phong.vert"},
    {GL_FRAGMENT_SHADER, "outline.frag"},
  });

  s_outlineshader = program::get("outline"); 

  s_outlineuniforms.resize(3);
  s_outlineuniforms[0] = glGetUniformLocation(s_outlineshader, "view");
  s_outlineuniforms[1] = glGetUniformLocation(s_outlineshader, "proj");
  s_outlineuniforms[2] = glGetUniformLocation(s_outlineshader, "model");

  std::vector<GLfloat> verts, norms;
  std::vector<GLuint> indcs;
  geometry::get_hexagon(verts, norms, indcs);

  GLuint tex;
  texloader::load("grass.png", GL_TEXTURE5, tex);
  texloader::load("water.png", GL_TEXTURE1, tex);
  texloader::load("desert.png", GL_TEXTURE2, tex);
  texloader::load("plains.png", GL_TEXTURE3, tex);
  texloader::load("mountain.png", GL_TEXTURE4, tex);

  s_tile = mesh::create(std::move(verts), std::move(norms), std::move(indcs), {
    program::get("phong")
  });

  // Give the tile some light.
  s_tile->m_light_material[0] = glm::vec3(0.4f, 0.4f, 0.4f);
  s_tile->m_light_material[1] = glm::vec3(0.5f, 0.5f, 0.5f);
  s_tile->m_light_material[2] = glm::vec3(0.3f, 0.35f, 0.3f);

  s_pawnmesh = mesh::create("pawn.obj", { program::get("phong") });
  s_rookmesh = mesh::create("rook.obj", { program::get("phong") });
  s_queenmesh = mesh::create("queen.obj", { program::get("phong") });
  s_bishopmesh = mesh::create("bishop.obj", { program::get("phong") });
  s_hutmesh = mesh::create("hut.obj", { program::get("phong") });


  light::apply(s_pawnmesh);
  light::apply(s_rookmesh);
  light::apply(s_queenmesh);
  light::apply(s_bishopmesh);
  light::apply(s_hutmesh);
  light::apply(s_tile);

  mesh::add_uniform(s_pawnmesh, "use_texture", &s_dontusetex);
  mesh::add_uniform(s_rookmesh, "use_texture", &s_dontusetex);
  mesh::add_uniform(s_queenmesh, "use_texture", &s_dontusetex);
  mesh::add_uniform(s_bishopmesh, "use_texture", &s_dontusetex);
  mesh::add_uniform(s_hutmesh, "use_texture", &s_dontusetex);

  mesh::bind_texture_data(s_tile, geometry::get_hexagontexcoords());
  mesh::add_uniform(s_tile, "tex_sampler", &s_texloc);
  mesh::add_uniform(s_tile, "use_texture", &s_usetex);

  GLFWwindow* w = gl::get_current_window();

  if (w) {
    glfwSetMouseButtonCallback(w, mouse_button_callback);
    glfwSetScrollCallback(w, mouse_scroll_callback);
    glfwSetKeyCallback(w, key_callback);
  }
}

void map::teardown() {
  delete s_hutmesh;
  delete s_bishopmesh;
  delete s_queenmesh;
  delete s_rookmesh;
  delete s_pawnmesh;
  delete s_tile;
}

void map::update(double delta) {
  if (sim_interface::poll()) {
    sim_interface::synch();
  }
}

void map::draw() {
  glm::ivec3 pos;
  const world_map::TileMap& tiles = sim_interface::get_map();

  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glStencilMask(0x00); // Make sure we don't update the stencil buffer while drawing the floor

  for (const auto& t : tiles) {
    pos = glm::ivec3(t.first.x, t.first.y, t.first.z);
    glm::vec2 world = glm_hex::cube_to_world(pos, 3);
    mesh::set_position(s_tile, glm::vec3(world.x, world.y, 0.0f));
    s_texloc = 0;
    switch (t.second.m_terrain_type) {
    case TERRAIN_TYPE::DESERT:
      s_texloc = 2;
      break;
    case TERRAIN_TYPE::GRASSLAND:
      s_texloc = 5;
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
    mesh::draw(s_tile);
  }

  const std::vector<Player>& players = sim_interface::get_players();
  const std::vector<Unit>& units = sim_interface::get_units();
  for (const auto& u : units) { 
    bool is_barbarian = false;
    for (const auto& p : players) {
      if (p.m_ai_type == AI_TYPE::BARBARIAN) {
        if (p.OwnsUnit(u.m_unique_id)) is_barbarian = true;
      }
    }
    pos = glm::ivec3(u.m_location.x, u.m_location.y, u.m_location.z);
    glm::vec2 world = glm_hex::cube_to_world(pos, 3);
    Mesh* todraw;

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    switch (u.m_unit_type) {
    case UNIT_TYPE::PHALANX:
      todraw = s_rookmesh;
      break;
    case UNIT_TYPE::SCOUT:
      todraw = s_pawnmesh;
      break;
    case UNIT_TYPE::WORKER:
      todraw = s_queenmesh;
      break;
    case UNIT_TYPE::ARCHER:
      todraw = s_bishopmesh;
      break;
    case UNIT_TYPE::UNKNOWN:
      todraw = s_pawnmesh;
      break;
    }
    mesh::set_scale(todraw, glm::vec3(1.0f, 1.0f, 1.0f));
    mesh::set_position(todraw, glm::vec3(world.x, world.y, 0.0f));

    if (is_barbarian) {
      glm::vec3 init_color = todraw->m_light_material[1]; // kd
      // Color dem red.
      todraw->m_light_material[1] = glm::vec3(0.8f, 0.3f, 0.3f);
      mesh::draw(todraw);
      todraw->m_light_material[1] = init_color;
    }
    else {
      mesh::draw(todraw);
    }

    if (s_selected == pos) {
      Camera* c = camera::get_current();
      assert(c);

      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
      glStencilMask(0x00);
      glDisable(GL_DEPTH_TEST);

      glUseProgram(s_outlineshader);

      // Move the mesh towards the camera.
      glm::vec3 old_position = todraw->m_position;
      glm::vec3 towards_camera = glm::normalize(c->m_position - old_position);
      mesh::set_position(todraw, old_position + (3.0f * towards_camera));
      mesh::update_transform(todraw);

      glUniformMatrix4fv(s_outlineuniforms[0], 1, GL_FALSE, glm::value_ptr(c->m_view));
      glUniformMatrix4fv(s_outlineuniforms[1], 1, GL_FALSE, glm::value_ptr(c->m_projection));
      glUniformMatrix4fv(s_outlineuniforms[2], 1, GL_FALSE, glm::value_ptr(todraw->m_matrix));

      mesh::raw_draw(todraw);

      // Set back to its original scale.
      mesh::set_position(todraw, old_position);
      //mesh::set_scale(todraw, glm::vec3(1.0f, 1.0f, 1.0f));
      mesh::update_transform(todraw);

      glStencilMask(0xFF);
      glEnable(GL_DEPTH_TEST);
    }
  }

  const std::vector<City>& cities = sim_interface::get_cities();
  for (const auto& c : cities) {
    bool is_barbarian = false;
    for (const auto& p : players) {
      if (p.m_ai_type == AI_TYPE::BARBARIAN && c.m_owner_id == p.m_id) {
        is_barbarian = true;
      }
    }
    pos = glm::ivec3(c.m_location.x, c.m_location.y, c.m_location.z);
    glm::vec2 world = glm_hex::cube_to_world(pos, 3);
    mesh::set_position(s_hutmesh, glm::vec3(world.x, world.y, 0.0f));
    if (is_barbarian) {
      glm::vec3 init_color = s_hutmesh->m_light_material[1]; // kd
      // Color dem red.
      s_hutmesh->m_light_material[1] = glm::vec3(1.0f, 0.0f, 0.0f);
      mesh::draw(s_hutmesh);
      s_hutmesh->m_light_material[1] = init_color;
    }
    else {
      mesh::draw(s_hutmesh);
    }
  }
}
