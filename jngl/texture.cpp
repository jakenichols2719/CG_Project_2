#include "jngl.h"

const int MAXLEN = 50;
typedef unsigned char byte;
typedef unsigned short int uint16;

// Read len bytes into memory
void readBytes(byte* mem, int* pos, int len, std::fstream* f) {
  for(int n=0; n<len; n++) mem[n] = f->get();
}

static int bytesToInt(byte* bytes, int offset, int size=4) {
  int val = 0;
  for(int n=0; n<size; n++) {
    val += bytes[n+offset]<<(n*8);
  }
  return val;
}

void Texture::loadFromBMP(std::string filename) {
  // Open file
  std::fstream file(filename);
  // Track position, helps debug
  int pos = 0;
  // Read header of BMP file
  const int hsize = 14;
  byte head[hsize];
  readBytes(head, &pos, hsize, &file);
  // Total file size, offset to pixel data
  int size = bytesToInt(head, 2);
  int offset = bytesToInt(head, 10);
  // Read info of BMP file
  int isize = offset-hsize;
  byte info[isize];
  readBytes(info, &pos, isize, &file);
  int img_width = bytesToInt(info,4); int img_height = bytesToInt(info,8);
  int bitsPerPixel = bytesToInt(info, 14, 2);
  if(bitsPerPixel != 24) {
    Debug::err("Please use 24-bit BMPs. " + filename);
  }

  // Read pixels
  int psize = size-offset;
  byte* px_bytes = new byte[psize];
  readBytes(px_bytes, &pos, psize, &file);

  // Create OpenGL texture
  glGenTextures(1,&tex_name);
  glBindTexture(GL_TEXTURE_2D, tex_name);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,img_width,img_height,0,GL_BGR,GL_UNSIGNED_BYTE,px_bytes);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Free memory
  delete[] px_bytes;
}