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

float sexp = 27.0; // Specular power

out vec4 frag_color;

void apply_phong(vec3 n, vec3 lpos) {
  // Notice lpos is not brought into view space. It is relative to the camera.
  vec3 l = normalize(lpos - eye_pos);
  vec3 v = normalize(-eye_pos);
  vec3 h = normalize(l + v);
  float costh = max(dot(n, h), 0.0);
  float costi = max(dot(n, l), 0.0);
  frag_color = vec4((ka + (kd + ks * pow(costh, sexp))) * costi, 1.0);
}

void apply_texture() {
  frag_color = texture(tex_sampler, tex_coords); 
}

void main() {
  // TODO: Clean this up.
  if (use_texture > 0) {
    apply_texture();
  }

  if (use_texture == 0) {
    for (int i = 0; i < lcount; ++i) {
      apply_phong(eye_norm, lpositions[i]);
    }
  }
}

