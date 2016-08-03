#version 330 

uniform vec3 h_color;

out vec4 frag_color;

void main() {
  frag_color = vec4(h_color, 0.6);
}
