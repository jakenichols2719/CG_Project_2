#version 450 core

uniform sampler2D screenTex;

out vec4 fragColor;

in VS_FS_INTERFACE {
  in vec2 vTex;
} fs_in;

void main() {
  fragColor = texture(screenTex, fs_in.vTex);
}
