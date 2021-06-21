#version 400 core

uniform vec4 col = vec4(1);

//  Input from previous shader
in VS_FS_INTERFACE {
  in vec4 vCol;
} fs_in;

//  Fragment color
layout (location=0) out vec4 Fragcolor;

void main()
{
   Fragcolor = col;
}
