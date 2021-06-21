#include "jngl.h"

VBO::VBO() {
  // Do nothing. VBOs won't draw by default, they must be constructed.
}
// Initialize with a regular float array
VBO::VBO(GLfloat data[], GLsizeiptr dat_arr_size, GLint count) {
  // Standard ArrayBuffer setup
  glGenBuffers(1,&vbo_name);
  if(!vbo_name) Debug::warn("VBO buffer couldn't be initialized");
  glBindBuffer(GL_ARRAY_BUFFER, vbo_name);
  glBufferData(GL_ARRAY_BUFFER, dat_arr_size, data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_name);
  pt_count = count;
}


void VBO::bind() {
  glBindBuffer(GL_ARRAY_BUFFER, vbo_name);
}
void VBO::unbind() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
bool VBO::verify() {
  if(vbo_name == 0) return false;
  GLint active = 0; glGetIntegerv((GL_ARRAY_BUFFER_BINDING), &active);
  if((GLint)vbo_name != active) return false;
  return true;
}
bool VBO::draw(GLenum mode) {
  // Draw if the current bound ArrayBuffer is this, and if valid
  // Don't draw if invalid
  if(!verify()) {
    Debug::warn("Attempting to draw invalid VBO");
    return false;
  }
  //std::cout << pt_count << std::endl;
  glDrawArrays(mode, 0, pt_count);
  return true;
}