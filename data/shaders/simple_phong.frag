#version 330 

in vec3 eye_pos, eye_norm;

uniform mat4 view;

// Support up to 10 lights?
uniform vec3 lpositions[10];
uniform int lcount;

vec3 ls = vec3(1.0, 1.0, 1.0); // white specular
vec3 ld = vec3(0.2, 0.7, 0.7); // dull white diffuse
vec3 la = vec3(0.3, 0.3, 0.3); // grey ambient color 

// Object surface properties
uniform vec3 ka = vec3(0.0, 0.0, 0.0); // Fully reflect ambient light
uniform vec3 kd = vec3(0.0, 0.0, 0.0); // Orange diffuse surface reflectance 
uniform vec3 ks = vec3(0.0, 0.0, 0.0); // Fully reflect specular light
float specular_exponent = 26.0; // Specular power

out vec4 frag_color;

void main() {
  vec3 ia = la * ka;

  // Raise light position to eye space
  for (int i = 0; i < lcount; ++i) {
    vec3 eye_light_position = vec3(view * vec4(lpositions[i], 1.0));
    vec3 distance = eye_light_position - eye_pos;
    vec3 direction = normalize(distance);
    float ddot = dot(direction, eye_norm);
    ddot = max(ddot, 0.0);
    vec3 id = ld * kd * ddot;

    vec3 eye_surface = normalize(-eye_pos);
    vec3 half_eye = normalize(eye_surface + direction);
    float sdot = max(dot(half_eye, eye_norm), 0.0);
    float sfac = pow(sdot, specular_exponent);
    vec3 is = ls * ks * sfac;

    frag_color += vec4(is + id + ia, 1.0);
  }
}

