#version 330 

uniform vec4 h_color;

out vec4 frag_color;

void main() {
  frag_color = h_color;
}
