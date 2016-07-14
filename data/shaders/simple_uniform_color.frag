#version 330 

uniform vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

out vec4 out_color;

void main() {
  out_color = color;
}
