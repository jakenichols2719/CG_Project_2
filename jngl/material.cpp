#include "jngl.h"

/*
 * If you have the files in a folder, you can set it with the path.
 * Requires formatted names:
 * base_col for base color
 * base_nrm for normal
 * base_amb for ambient
 * base_diff for diffuse
 * base_spec for specular
 * Convenient sometimes.
*/
void Material::setFromFolder(std::string folder, std::string basename) {
  std::string path = folder + "/" + basename + "_";
  color = Texture(path+"col.bmp");
  normal = Texture(path+"nrm.bmp");
  ambient = Texture(path+"amb.bmp");
  diffuse = Texture(path+"diff.bmp");
  specular = Texture(path+"spec.bmp");
}

void Material::bind() {
  // Bind textures to texture units
  glActiveTexture(GL_TEXTURE0); color.bind();
  glActiveTexture(GL_TEXTURE1); normal.bind();
  glActiveTexture(GL_TEXTURE2); ambient.bind();
  glActiveTexture(GL_TEXTURE3); diffuse.bind();
  glActiveTexture(GL_TEXTURE4); specular.bind();
}

void Material::unbind() {
  glActiveTexture(GL_TEXTURE0); color.unbind();
  glActiveTexture(GL_TEXTURE1); normal.unbind();
  glActiveTexture(GL_TEXTURE2); ambient.unbind();
  glActiveTexture(GL_TEXTURE3); diffuse.unbind();
  glActiveTexture(GL_TEXTURE4); specular.unbind();
}