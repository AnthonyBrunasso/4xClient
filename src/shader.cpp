#include "shader.h"

#include <iostream>

namespace shader {
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

  // Detach the shaders after a successful link.
  for (auto shader : shaders) {
    glDetachShader(p, shader);
  }

  return p;
}
