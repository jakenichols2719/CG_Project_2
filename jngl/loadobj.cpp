// Loading object files is a time, so it gets its own cpp file.
#include "jngl.h"

enum LineType {COMMENT, MATERIAL, VERTEX, TEXTURE, NORMAL, USEMTL, SMOOTH, FACE, UNKNOWN};
const int MAXLEN = 256;

LineType getType(char* line) {
  if(line[0] == '#' || line[0] == 'o') return COMMENT;
  else if(line[0] == 'v' && line[1] == ' ') return VERTEX;
  else if(line[0] == 'v' && line[1] == 't') return TEXTURE;
  else if(line[0] == 'v' && line[1] == 'n') return NORMAL;
  else if(line[0] == 's') return SMOOTH;
  else if(line[0] == 'f') return FACE;
  else if(line[0] == 'm') return MATERIAL;
  else if(line[0] == 'u') return USEMTL;
  else return UNKNOWN;
}

// Adds values for vertices, texture coordinates, normals to a vector
static void readValues(std::vector<float>* v, std::string line, int ct) {
  int idx = line.find(' '); // Find first space
  for(int n=0; n<ct; n++) {
    idx++; // Move to next spot (don't want to start on space)
    int next = line.find(' ', idx);
    std::string strval = line.substr(idx, next-idx);
    float val = std::stof(strval);
    v->push_back(val);
    idx = next;
  }
}

// Read points from a line into a vector
static int readPoints(std::vector<int>* v, std::string line, int attrib_ct) {
  int space_idx = line.find(' ');
  int point_ct = 0;
  // Iterate through chunks of line
  while((size_t)space_idx != std::string::npos) {
    // Get "chunk" between spaces
    space_idx++;
    int space_next = line.find(' ', space_idx);
    std::string stridx = line.substr(space_idx, space_next-space_idx);
    // Go through chunk to get index values
    int s1 = stridx.find('/');
    int s2 = stridx.find('/',s1+1);
    v->push_back(std::stoi(stridx.substr(0,s1)));
    v->push_back(std::stoi(stridx.substr(s1+1,s2)));
    v->push_back(std::stoi(stridx.substr(s2+1,std::string::npos)));
    point_ct++;
    // Next chunk
    space_idx = space_next;
  }
  return point_ct;
}

void VBO::loadFromOBJ(std::string filename) {
  // Setup data vectors
  std::vector<float> vertices;
  std::vector<float> texcoords;
  std::vector<float> normals;
  std::vector<int> indices; int index_ct = 0; int attrib_ct = 3;
  // Open file
  std::fstream file (filename.c_str());
  char line[MAXLEN];
  // Iterate through lines of file, read in raw data
  int err = 0;
  while(!file.eof()) {
    file.getline(line, MAXLEN, '\n');
    std::string strline (line);
    if(strline=="") {
      err++;
      if(err>=10) {
        std::cout << filename << " machine broke" << std::endl;
        std::cout << file.eof() << " " << file.fail() << " " << file.bad() << std::endl;
        exit(1);
      }
    }
    switch(getType(line)) {
      case COMMENT: {
        break;
      }
      case MATERIAL: {
        break;
      }
      case VERTEX: {
        readValues(&vertices, line, 3);
        break;
      }
      case TEXTURE: {
        readValues(&texcoords, line, 2);
        break;
      }
      case NORMAL: {
        readValues(&normals, line, 3);
        break;
      }
      case SMOOTH: {
        break;
      }
      case FACE: {
        index_ct += readPoints(&indices, line, attrib_ct);
        break;
      }
      case USEMTL: {
        break;
      }
      default: {
        break;
      }
    }
  }

  /*
   * For each index, there is a vertex (4 floats), tex coord (2 floats), normal (3 floats)
   * Assuming all are present for now.
  */
  float* vertex_buffer = new float[index_ct*4*2*3]; float* vptr = vertex_buffer;
  // Populate buffers based on data
  // One point takes 3 indices for vertex, texture, normal
  pt_count = 0;
  std::cout << filename << ": " << indices.size() << std::endl;
  for(int n=0; n<(int)indices.size(); n+=3) {
    int vtx = indices[n]; int tex = indices[n+1]; int nrm = indices[n+2];
    // Vertex
    int vidx = (vtx-1)*3;
    //std::cout << vertices[vidx+0] << ", " << vertices[vidx+1] << ", " << vertices[vidx+2] << " | ";
    *vptr++ = vertices[vidx+0]; // X
    *vptr++ = vertices[vidx+1]; // Y
    *vptr++ = vertices[vidx+2]; // Z
    *vptr++ = 1.0;              // W
    // Texture
    int tidx = (tex-1)*2;
    //std::cout << texcoords[tidx+0] << ", " << texcoords[tidx+1] << " | ";
    *vptr++ = texcoords[tidx+0];
    *vptr++ = texcoords[tidx+1];
    // Normal
    int nidx = (nrm-1)*3;
    //std::cout << normals[nidx+0] << ", " << normals[nidx+1] << ", " << normals[nidx+2];
    *vptr++ = normals[nidx+0];
    *vptr++ = normals[nidx+1];
    *vptr++ = normals[nidx+2];
    // Index
    pt_count++;
    //std::cout << std::endl;
  }
  // Print buffers
/*
  for(int idx=0; idx<index_ct; idx++) {
    std::cout << idx << ": ";
    int vbuf_idx = idx*9;
    for(int n=0; n<9; n++) {
      std::cout << vertex_buffer[vbuf_idx] << ", ";
      vbuf_idx++;
    }
    std::cout << std::endl;
  }
*/

  // Generate buffers
  glGenBuffers(1,&vbo_name);
  if(!vbo_name) Debug::warn("VBO couldn't be initalized");
  // Fill VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo_name);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*index_ct*4*2*3, vertex_buffer, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  pt_count = index_ct;
  // Free memory
  delete[] vertex_buffer;
}