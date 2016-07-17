#include "program.h"

#include <unordered_map>
#include <GL/gl3w.h>

#include "shader.h"

namespace program {

#define PROGRAM_LOG "program.log"

  typedef std::vector<std::pair<GLenum, std::string> > ShaderNames;
  typedef std::vector<GLuint> ShaderIds;

  struct ProgramData {
    GLuint m_program;
    ShaderNames m_shader_names;
    ShaderIds m_shader_ids;
  };

  typedef std::vector<std::function<void(GLuint, GLuint)> > SubMap;
  typedef std::unordered_map<std::string, ProgramData> ProgramMap;

  SubMap s_rebuild_subs;
  ProgramMap s_programs;
}

void program::build(const std::string& name, const ShaderNames& shaders) {
  ProgramData pdata;

  pdata.m_shader_names = shaders;
  for (const auto& s : pdata.m_shader_names) {
    GLuint sc = shader::compile_from_file(s.first, s.second.c_str());
    pdata.m_shader_ids.push_back(sc);
  }

  pdata.m_program = shader::link(pdata.m_shader_ids);

  // Only add the program if linking succeeded.
  
  if (pdata.m_program) {
    s_programs[name] = pdata;
  }
}

void program::remove(const std::string& name) {
  if (s_programs.find(name) == s_programs.end()) return;
  const ProgramData& data = s_programs[name];
  glDeleteProgram(data.m_program);
  for (auto s : data.m_shader_ids) {
    glDeleteShader(s);
  }
  s_programs.erase(name);
}

GLuint program::get(const std::string& name) {
  if (s_programs.find(name) == s_programs.end()) return 0;
  return s_programs[name].m_program;
}

// Force a rebuild on a programj
void program::rebuid(const std::string& name) {
  if (s_programs.find(name) == s_programs.end()) return;
  // Grab a copy because it will be deleted in program::remove.
  ProgramData data = s_programs[name];
  // Delete the original program.
  program::remove(name);
  // Rebuild the shader program.
  program::build(name, data.m_shader_names);
  const ProgramData& new_data = s_programs[name];
  for (auto s : s_rebuild_subs) {
    s(data.m_program, new_data.m_program);
  }
}

void program::sub_rebuild(std::function<void(GLuint, GLuint)> sub) {
  s_rebuild_subs.push_back(sub);
}
