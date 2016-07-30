#version 330

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
// 1 is usually saved for normals, so make texture location 2.
layout (location = 2) in vec2 vt; // per-vertex texture co-ords

uniform mat4 view, proj, model;

out vec3 eye_pos, eye_norm;
out vec2 texture_coordinates;

void main() {
	texture_coordinates = vt;
	gl_Position = proj * view * model * vec4 (vertex_position, 1.0);
}
