#include "shader.h"
#include "log.h"


#include <iostream>
#include <sstream>

namespace shader {

#define SHADER_LOG "shader.log"

  const char* BASIC_V =
  "#version 400\n"
  "in vec3 vp;"
  "void main() {"
  "  gl_Position = vec4(vp, 1.0);"
  "}";

  const char* BASIC_F =
  "#version 400\n"
  "out vec4 frag_color;"
  "void main() {"
  "  frag_color = vec4(0.5, 0.0, 0.5, 1.0);"
  "}";

  const char* load_shader(const char* file) {
    return "";
  }

  const char* gl_type_to_string(GLenum type) {
    switch (type) {
      case GL_BOOL: return "bool";
      case GL_INT: return "int";
      case GL_FLOAT: return "float";
      case GL_FLOAT_VEC2: return "vec2";
      case GL_FLOAT_VEC3: return "vec3";
      case GL_FLOAT_VEC4: return "vec4";
      case GL_FLOAT_MAT2: return "mat2";
      case GL_FLOAT_MAT3: return "mat3";
      case GL_FLOAT_MAT4: return "mat4";
      case GL_SAMPLER_2D: return "sampler2D";
      case GL_SAMPLER_3D: return "sampler3D";
      case GL_SAMPLER_CUBE: return "samplerCube";
      case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
      default: break;
    }
    return "unknown";
  }
}

GLuint shader::compile_from_file(GLenum type, const char* file) {
  const char* source = load_shader(file);
  return compile_from_buffer(type, source);
}

GLuint shader::compile_from_buffer(GLenum type, const char* buffer) {
  GLuint s = glCreateShader(type);

  glShaderSource(s, 1, &buffer, NULL);
  glCompileShader(s);
 
  GLint success = 0;
  glGetShaderiv(s, GL_COMPILE_STATUS, &success);

  // Shader failed to compile.
  if (success == GL_FALSE) {
    GLint max_length = 0;
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &max_length);

    std::vector<GLchar> error_log(max_length);
    glGetShaderInfoLog(s, max_length, &max_length, &error_log[0]);

    // Report error to out.
    for (auto c : error_log) {
      std::cout << c;
    }

    std::cout << std::endl;

    glDeleteShader(s);
    return 0;
  }

  // Shader succeeded.
  return s;
}
GLuint shader::link(std::vector<GLuint> shaders) {
  GLuint p = glCreateProgram();

  // Attach the shaders to be linked.
  for (auto shader : shaders) {
    glAttachShader(p, shader);
  }

  // Link them.
  glLinkProgram(p);

  GLint linked = 0;
  glGetProgramiv(p, GL_LINK_STATUS, (int*)&linked);

  // Shader failed to compile.
  if (linked == GL_FALSE) {
    GLint max_length = 0;
    glGetProgramiv(p, GL_INFO_LOG_LENGTH, &max_length);

    std::vector<GLchar> error_log(max_length);
    glGetShaderInfoLog(p, max_length, &max_length, &error_log[0]);

    // Report error to out.
    for (auto c : error_log) {
      std::cout << c;
    }

    std::cout << std::endl;

    glDeleteProgram(p);
    for (auto shader : shaders) {
      glDeleteShader(shader);
    }

    return 0;
  }

  // Log before the shaders are detached.
  logging::write(SHADER_LOG, get_debug(p));

  // Detach the shaders after a successful link.
  for (auto shader : shaders) {
    glDetachShader(p, shader);
  }

  return p;
}

std::string shader::get_debug(GLuint program) {
  std::ostringstream ss;
  ss << "shader program: " << program << std::endl;
  GLint params = -1;
  glGetProgramiv(program, GL_LINK_STATUS, &params);
  ss << "GL_LINK_STATUS = " << params << std::endl;
  glGetProgramiv(program, GL_ATTACHED_SHADERS, &params);
  ss << "GL_ATTACHED_SHADERS = " << params << std::endl;
  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &params);
  ss << "GL_ACTIVE_ATTIRBUTES = " << params << std::endl;

  for (GLuint i = 0; i < (GLuint)params; ++i) {
    char name[64];
    int max_length = 64;
    int actual_length = 0;
    int size = 0;
    GLenum type;
    glGetActiveAttrib(program, i, max_length, &actual_length, &size, &type, name);
    if (size > 1) {
      for (int j = 0; j < size; ++j) {
        char long_name[64];
        sprintf(long_name, "%s[%i]", name, j);
        int location = glGetAttribLocation(program, long_name);
        ss << "(" << i << ") type: " << gl_type_to_string(type)
           << " name: " << long_name
           << " location: " << location << std::endl;
      }
    }
    else {
      int location = glGetAttribLocation(program, name);
       ss << "(" << i << ") type: " << gl_type_to_string(type)
         << " name: " << name 
         << " location: " << location << std::endl;
    }
  }

  glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &params);
  ss << "GL_ACTIVE_UNIFORMS = " << params << std::endl;
  for (GLuint i = 0; i < (GLuint)params; ++i) {
    char name[64];
    int max_length = 64;
    int actual_length = 0;
    int size = 0;
    GLenum type;
    glGetActiveUniform(program, i, max_length, &actual_length, &size, &type, name);
    if (size > 1) {
      for (int j = 0; j < size; ++j) {
        char long_name[64];
        sprintf(long_name, "%s[%i]", name, j);
        int location = glGetUniformLocation(program, long_name);
        ss << "(" << i << ") type: " << gl_type_to_string(type)
           << " name: " << long_name
           << " location: " << location << std::endl;

      }
    }
    else {
      int location = glGetUniformLocation(program, name);
       ss << "(" << i << ") type: " << gl_type_to_string(type)
         << " name: " << name 
         << " location: " << location << std::endl;
    }
  }
  return ss.str();
}
