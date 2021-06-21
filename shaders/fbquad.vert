#version 450 core

//  Transformation matrices
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ViewMatrix;
uniform mat3 NormalMatrix;

//  Vertex attributes (input)
layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec2 Texture;
layout(location = 2) in vec3 Normal;

//  Output to next shader
out VS_FS_INTERFACE {
  out vec2 vTex;
} vs_out;

void main()
{
   mat4 MVP = ProjectionMatrix * ModelViewMatrix;
   gl_Position = MVP * Vertex;
   vs_out.vTex = Texture;
}
