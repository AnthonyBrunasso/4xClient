#version 330 

in vec3 eye_pos, eye_norm;

uniform mat4 view;

// Support up to 10 lights?
uniform vec3 lpositions[10];
uniform int lcount;

// Object surface properties
uniform vec3 ka = vec3(0.0, 0.0, 0.0);
uniform vec3 kd = vec3(0.0, 0.0, 0.0);
uniform vec3 ks = vec3(0.0, 0.0, 0.0);

in vec2 tex_coords;
uniform sampler2D tex_sampler;
uniform int use_texture = 0;

float sexp = 2007.0; // Specular power

out vec4 frag_color;

void get_phong(vec3 n, vec3 lpos, out vec3 amb_diff, out vec3 spec) {
  // Notice lpos is not brought into view space. It is relative to the camera.
  vec3 l = normalize(lpos - eye_pos);
  vec3 v = normalize(-eye_pos);
  vec3 h = normalize(l + v);
  float costh = max(dot(n, h), 0.0);
  float costi = max(dot(n, l), 0.0);
  amb_diff = ka + kd * costi;
  spec = ks * pow(costh, sexp);
}

void main() {
  // TODO: Clean this up.
  vec4 tex = vec4(1.0, 1.0, 1.0, 1.0);
  if (use_texture > 0) {
    tex = texture(tex_sampler, tex_coords); 
  }

  vec3 amb_diff = vec3(0.2, 0.2, 0.2);
  vec3 spec = vec3(0.2, 0.2, 0.2);
  for (int i = 0; i < lcount; ++i) {
    get_phong(eye_norm, lpositions[i], amb_diff, spec);
  }

  frag_color = vec4(amb_diff, 1.0) * tex + vec4(spec, 1.0);
}

