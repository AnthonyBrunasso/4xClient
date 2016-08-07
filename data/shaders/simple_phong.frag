#version 330 

in vec3 eye_pos, eye_norm, light_norm;

uniform mat4 view;

// Support up to 10 lights?
uniform vec3 lpositions[10];
uniform int lcount;

// Temporary for bump mapping
float bump_density = 2.5;
float bump_size = 0.10;

// Object surface properties
uniform vec3 ka = vec3(0.0, 0.0, 0.0);
uniform vec3 kd = vec3(0.0, 0.0, 0.0);
uniform vec3 ks = vec3(0.0, 0.0, 0.0);

in vec2 tex_coords;
uniform sampler2D tex_sampler;
uniform int use_texture = 0;

float sexp = 200.0; // Specular power

out vec4 frag_color;

void get_phong(vec3 n, vec3 lpos, out vec3 amb_diff, out vec3 spec) {
  // Notice lpos is not brought into view space. It is relative to the camera.
  vec3 l = normalize(lpos - eye_pos);
  vec3 v = normalize(-eye_pos);
  vec3 h = normalize(l + v);
  float costh = max(dot(n, h), 0.0);
  float costi = max(dot(n, l), 0.0);
  amb_diff = ka + kd * costi;
  spec = ks * pow(costh, sexp);
}

void main() {
  // TODO: Clean this up.
  vec4 tex = vec4(1.0, 1.0, 1.0, 1.0);
  if (use_texture > 0) {
    tex = texture(tex_sampler, tex_coords); 
    //jhk

    vec3 amb_diff = vec3(0.2, 0.2, 0.2);
    vec3 spec = vec3(0.2, 0.2, 0.2);
    get_phong(eye_norm, lpositions[0], amb_diff, spec);

    vec3 lit_color;
    vec2 c = bump_density * tex.st;
    vec2 p = fract(c) - vec2(0.5);

    float d, f;
    d = dot(p, p);
    f = inversesqrt(d + 1.0);

    if (d >= bump_size) {
      p = vec2(0.0);
      f = 1.0;
    }

    vec3 norm_delta = vec3(p.x, p.y, 1.0) * f;
    lit_color = vec3(tex) * max(dot(norm_delta, light_norm), 0.0);
    vec3 reflect_dir = reflect(light_norm, norm_delta);

    lit_color = min(lit_color + spec, vec3(1.0));
    frag_color = vec4(lit_color, 1.0);
  }
  else {
    vec3 amb_diff = vec3(0.2, 0.2, 0.2);
    vec3 spec = vec3(0.2, 0.2, 0.2);
    for (int i = 0; i < lcount; ++i) {
      get_phong(eye_norm, lpositions[i], amb_diff, spec);
    }

    frag_color = vec4(amb_diff, 1.0) * tex + vec4(spec, 1.0);
  }
}

