#version 450 core

uniform vec3 light_pos;
uniform float far;

in vec4 frag_pos;

void main() {
  // Distance between fragment and light source
  float dist = length(frag_pos.xyz-light_pos);
  // Map [0,1]
  dist = dist/far;
  // Write as depth
  gl_FragDepth = dist;
}
