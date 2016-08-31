#version 330 
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture;
layout(location = 3) in vec3 vertex_tangent;

// Support up to 10 lights?
uniform vec3 lpositions[10];
uniform int lcount;

uniform mat4 view, proj, model;

out vec3 eye_pos, eye_norm;
out vec2 tex_coords;

void main() {
  tex_coords = vertex_texture;

  mat4 model_view = view * model;

  eye_pos = vec3(model_view * vec4(vertex_position, 1.0));
  eye_norm = normalize(vec3(model_view * vec4(vertex_normal, 0.0)));
  gl_Position = proj * vec4(eye_pos, 1.0);
}

