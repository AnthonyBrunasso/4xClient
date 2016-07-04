#version 410
in vec3 position;
int main() {
  gl_Position = vec4(position, 1.0);
};
