#version 330

in vec3 eye_pos, eye_norm;
in vec2 st; // Texture coords passed from the vert shader

uniform sampler2D diffuse_map;

uniform vec3 lpositions[10];
uniform int lcount;

uniform float specular_exponent;

out vec4 frag_color;

vec3 ls = vec3(1.0, 1.0, 1.0); // white specular
vec3 ld = vec3(0.2, 0.7, 0.7); // dull white diffuse
vec3 la = vec3(0.3, 0.3, 0.3); // grey ambient color 

void main() {
  vec4 sample = texture(diffuse_map, st);
  vec3 kd = sasmple.rgb;
  vec3 surface_to_light_eye = normalize(lpositions[0] - eye_pos);
  float dp = dot(eye_norm, surface_to_light_eye);
  vec3 id = kd * ld * dp;

  frag_color = vec4(id + is + ia, sample.a);
}

