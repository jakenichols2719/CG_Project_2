#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform sampler2D tex_bump;
uniform float bump_intensity = 0.05;

// Matrices
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;

in TES_GS_INTERFACE {
  vec4 vPos;
  vec2 vTex;
  vec3 vNrm;
} tes_out[];

out GS_FS_INTERFACE {
  vec4 vPos;
  vec2 vTex;
  vec3 vNrm;
} gs_out;

float bump(vec2 tc) {
  float val = texture(tex_bump, tc).r*bump_intensity;
  return val;
}

void main() {
  for(int n=0; n<3; n++) {
    float bumpVal = bump(tes_out[n].vTex);
    vec4 vertex = tes_out[n].vPos + bumpVal*vec4(tes_out[n].vNrm,0);
    gs_out.vPos = ModelMatrix * vertex;
    gs_out.vTex = tes_out[n].vTex;
    gs_out.vNrm = tes_out[n].vNrm;
    mat4 MVP = ProjectionMatrix * ModelViewMatrix;
    gl_Position = MVP * vertex;
    EmitVertex();
  }
  EndPrimitive();
}
