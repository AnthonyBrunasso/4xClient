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

float sexp = 27.0; // Specular power

out vec4 frag_color;

vec3 phong(vec3 n, vec3 lpos) {
  // Notice lpos is not brought into view space. It is relative to the camera.
  vec3 l = normalize(lpos - eye_pos);
  vec3 v = normalize(-eye_pos);
  vec3 h = normalize(l + v);
  float costh = max(dot(eye_norm, h), 0.0);
  float costi = max(dot(eye_norm, l), 0.0);
  return (ka + (kd + ks * pow(costh, sexp))) * costi;
}

void main() {
  // Raise light position to eye space
//  frag_color = vec4(phong(eye_norm, lpositions[0]), 1.0);
  for (int i = 0; i < lcount; ++i) {
    frag_color += vec4(phong(eye_norm, lpositions[i]), 1.0);
  }
}

