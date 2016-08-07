#version 330 
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture;
layout(location = 3) in vec3 vertex_tangent;

// Support up to 10 lights?
uniform vec3 lpositions[10];
uniform int lcount;

uniform mat4 view, proj, model;

out vec3 eye_pos, eye_norm, light_norm;
out vec2 tex_coords;

void main() {
  tex_coords = vertex_texture;

  mat4 model_view = view * model;

  // Create orthonormal tanget space matrix.
  vec3 n = normalize(vec3(model_view * vec4(vertex_normal, 0.0)));
  vec3 t = normalize(vec3(model_view * vec4(vertex_tangent, 0.0)));
  vec3 b = cross(n, t);

  if (lcount > 0) {
    vec3 v;
    v.x = dot(lpositions[0], t);
    v.y = dot(lpositions[0], b);
    v.z = dot(lpositions[0], n);
    light_norm = normalize(v);
  }

  eye_pos = vec3(model_view * vec4(vertex_position, 1.0));
  eye_norm = n;
  gl_Position = proj * vec4(eye_pos, 1.0);
}

