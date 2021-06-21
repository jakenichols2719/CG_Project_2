#include "jngl.h"

static char* readFile(const char* filename) {
  using namespace std; // fstream
  // Open file
  fstream file (filename);
  if(!file.is_open()) {
    cout << "Couldn't open file " << filename << endl;
  }
  // Get file size
  file.seekg(0,ios::end);
  int size = file.tellg();
  file.seekg(0,ios::beg);

  char* buffer = new char[size];
  file.get(buffer,size,'\0');

  file.close();

  return buffer;
}

// Print a log for a shader program (gives errors)
static void printLog(int prog) {
  GLint len = 0;
  glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
  if(len > 1) {
    char* log = new char[len];
    glGetProgramInfoLog(prog, len, nullptr, log);
    std::cout << log << std::endl;
  }
  GLint status = 0;
  glGetProgramiv(prog, GL_LINK_STATUS, &status);
  if(!status) exit(0);
}

// Attach a new shader of type to a program
static void attachNewShader(int to_prog, GLenum type, const char* filename) {
  int shader = glCreateShader(type);
  // Compile source into shader
  char* src  = readFile(filename);
  glShaderSource(shader, 1, (const char**)&src, nullptr);
  glCompileShader(shader);
  // Attach shader to program
  glAttachShader(to_prog, shader);
  // free memory
  delete[] src;
}

Shader::Shader() { }


// Functions to set shader filenames
void Shader::setShaderFile(GLenum type, std::string filename) {
  switch(type) {
    case GL_VERTEX_SHADER: {fn_vertex_shader = filename; break;}
    case GL_FRAGMENT_SHADER: {fn_fragment_shader = filename; break;}
    case GL_TESS_CONTROL_SHADER: {fn_tess_control_shader = filename; break;}
    case GL_TESS_EVALUATION_SHADER: {fn_tess_eval_shader = filename; break;}
    case GL_GEOMETRY_SHADER: {fn_geometry_shader = filename; break;}
    default: return;
  }
}

/*
 * Compile shader program
 * Selects non-null filenames and outputs
 * Fails if there is not a vertex and fragment shader
 * Returns compilation success/failure
*/
bool Shader::compile() {
  // Check for vert/frag
  if(fn_vertex_shader=="" || fn_fragment_shader=="") return false;
  GLint program = glCreateProgram();
  // Vert/Frag shaders (basics)
  attachNewShader(program, GL_VERTEX_SHADER, fn_vertex_shader.c_str());
  attachNewShader(program, GL_FRAGMENT_SHADER, fn_fragment_shader.c_str());
  // Rest (optional)
  if(fn_tess_control_shader != "") attachNewShader(program, GL_TESS_CONTROL_SHADER, fn_tess_control_shader.c_str());
  if(fn_tess_eval_shader != "") attachNewShader(program, GL_TESS_EVALUATION_SHADER, fn_tess_eval_shader.c_str());
  if(fn_geometry_shader != "") attachNewShader(program, GL_GEOMETRY_SHADER, fn_geometry_shader.c_str());
  // Link
  glLinkProgram(program);
  // Check errors (make your own!)
  printLog(program);
  shader_name = program;
  return true;
}

// Sets active program (bind for consistent phrasing)
void Shader::bind() {
  glUseProgram(shader_name);
}

// Deactivates this program
void Shader::unbind() {
  glUseProgram(0);
}

// Verifies that shader is valid and active
bool Shader::verify() {
  if(shader_name == 0) return false;
  GLint active; glGetIntegerv(GL_CURRENT_PROGRAM, &active);
  if((GLint)shader_name != active) return false;
  return true;
}

// Sets a uniform for this shader
void Shader::setUniform(UType type, std::string name, void* val_ptr) {
  const char* cname = name.c_str();
  GLint id = glGetUniformLocation(shader_name, cname);
  switch(type) {
    case U_INT:  {
      GLint* ptr = (GLint*)val_ptr; GLint val = *ptr;
      glUniform1i(id, val);
      break;
    }
    case U_FLOAT: {
      GLfloat* ptr = (GLfloat*)val_ptr; GLfloat val = *ptr;
      glUniform1f(id, val);
      break;
    }
    case U_VEC2: {
      GLfloat* ptr = (GLfloat*)val_ptr;
      glUniform2fv(id, 1, ptr);
      break;
    }
    case U_VEC3: {
      GLfloat* ptr = (GLfloat*)val_ptr;
      glUniform3fv(id, 1, ptr);
      break;
    }
    case U_VEC4: {
      GLfloat* ptr = (GLfloat*)val_ptr;
      glUniform4fv(id, 1, ptr);
      break;
    }
    case U_MAT3: {
      GLfloat* ptr = (GLfloat*)val_ptr;
      glUniformMatrix3fv(id, 1, 1, ptr);
      break;
    }
    case U_MAT4: {
      GLfloat* ptr = (GLfloat*)val_ptr;
      glUniformMatrix4fv(id, 1, 1, ptr);
      break;
    }
    default: {
      std::cout << "Attepted to set uniform with invalid UType" << std::endl;
      break;
    }
  }
}

void Shader::setMTL(Material m) {
  bind();
  m.bind();
  glUniform1i(glGetUniformLocation(shader_name,"tex_col"), 0);
  glUniform1i(glGetUniformLocation(shader_name,"tex_nrm"), 1);
  glUniform1i(glGetUniformLocation(shader_name,"tex_amb"), 2);
  glUniform1i(glGetUniformLocation(shader_name,"tex_diff"), 3);
  glUniform1i(glGetUniformLocation(shader_name,"tex_spec"), 4);
  glUniform1i(glGetUniformLocation(shader_name,"depthMap"), 5);
}

/*
//
//  Create Shader Program with Attribute Locations
//
int CreateShaderProgAttr(char* VertFile,char* FragFile,char* Name[])
{
   int k;
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   if (VertFile) CreateShader(prog,GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   if (FragFile) CreateShader(prog,GL_FRAGMENT_SHADER,FragFile);
   //  Set names
   for (k=0;Name[k];k++)
      if (Name[k][0])
         glBindAttribLocation(prog,k,Name[k]);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

//
//  Create Shader Program consisting of vertex, geometry and fragment shader
//
int CreateShaderProgGeom(const char* vert,const char* geom,const char* frag)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   if (vert) CreateShader(prog,GL_VERTEX_SHADER,vert);
#ifdef GL_GEOMETRY_SHADER
   //  Create and compile geometry shader
   if (geom) CreateShader(prog,GL_GEOMETRY_SHADER,geom);
#else
   if (geom) Fatal("Geometry Shader not supported\n");
#endif
   //  Create and compile fragment shader
   if (frag) CreateShader(prog,GL_FRAGMENT_SHADER,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}
*/