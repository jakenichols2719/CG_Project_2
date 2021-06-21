#version 400 core

//  Transformation matrices
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ViewMatrix;
uniform mat3 NormalMatrix;

// Light position
uniform vec4 Position;

//  Vertex attributes (input)
layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec2 Texture;
layout(location = 2) in vec3 Normal;
vec4 Color = vec4(.75);

//  Output to next shader
out VS_TCS_INTERFACE {
  vec4 vPos;
  vec2 vTex;
  vec3 vNrm;
} vs_out;

void main()
{
   //mat4 MVP = ProjectionMatrix * ModelViewMatrix;
   //gl_Position = MVP * Vertex;
   //vs_out.vCol = phong();
   vs_out.vPos = Vertex;
   vs_out.vTex = Texture;
   vs_out.vNrm = Normal;
}
