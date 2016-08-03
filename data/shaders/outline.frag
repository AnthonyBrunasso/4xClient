#version 330 

uniform vec3 h_color = vec3(0.0, 0.8, 0.3);

out vec4 frag_color;

void main() {
  frag_color = vec4(h_color, 0.6);
}
