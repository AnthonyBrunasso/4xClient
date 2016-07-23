#include "light.h"
#include "mesh.h"
#include "log.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <string>

namespace light {

#define LIGHT_LOG "light.log"

  static const uint32_t LIGHT_LIMIT = 10;

  std::vector<glm::vec3> s_lights;
  int s_lightcount = 0;
}

void light::add(const glm::vec3& pos) {
  s_lights.push_back(pos);
  ++s_lightcount;
}

const std::vector<glm::vec3>& light::get() {
  return s_lights;
}

void light::apply(Mesh* m) {
  // Haha.
  assert(s_lights.size() <= LIGHT_LIMIT);

  for (uint32_t i = 0; i < s_lights.size(); ++i) {
    char name[16]; 
    sprintf(name, "lpositions[%d]", i);
    std::string logline = "Applying light source to: ";
    logline += name;
    logline += "\n";
    logging::write(LIGHT_LOG, logline);
    mesh::add_uniform(m, name, GL_FLOAT_VEC3, &s_lights[i][0]);
  }

  mesh::add_uniform(m, "lcount", &s_lightcount);
}
