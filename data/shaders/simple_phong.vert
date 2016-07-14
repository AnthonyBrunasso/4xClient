#version 330 
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;

uniform mat4 view, proj, model;

out vec3 eye_pos, eye_norm;

void main() {
  eye_pos = vec3(view * model * vec4(vertex_position, 1.0));
  eye_norm = vec3(view * model * vec4(vertex_normal, 0.0));
  gl_Position = proj * vec4(eye_pos, 1.0);
}

