#version 450 core

layout(location = 0) in vec4 Vertex;

uniform mat4 ModelMatrix;

void main() {
  gl_Position = ModelMatrix * Vertex;
}
