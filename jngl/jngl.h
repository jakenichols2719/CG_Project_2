// jngl.h
// Defines all behavior for the JNGL library
// James Nichols/28

#ifndef jngl
#define jngl

// C++ inclusions
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <math.h>
// Get rid of these eventually
#include <stdarg.h>

// OpenGL inclusions
#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

/*
 * Layer -1: Debugging
 * We all need some debug in our life.
*/
class Debug {
public:
  static void print(std::string print_msg);
  static void warn(std::string warn_msg);
  static void err(std::string err_msg);
  static bool GLerr(std::string err_msg);
  static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
};

/*
 * Layer 0: Math Classes
 * These classes provide no actual OpenGL functionality, but
 * provide handy shortcuts for math (common matrices, vectors).
*/
// Vectors 2-4
struct vec2 {
  union {
    float ptr[2];
    struct {
      float x;
      float y;
    };
  };
  vec2() : x(0), y(0) {};
  vec2(float val) : x(val), y(val) {};
  vec2(float a, float b) : x(a), y(b) {};
  void print();
  static vec2   normalize(vec2 a);
  static float  dot(vec2 a, vec2 b);
  vec2 operator + (vec2 const& b);
  vec2 operator - (vec2 const& b);
  vec2 operator * (float const& s);
};

struct vec3 {
  union {
    float ptr[3];
    struct {
      union { float x; float r; };
      union { float y; float g; };
      union { float z; float b; };
    };
  };
  vec3() : x(0), y(0), z(0) {};
  vec3(float val) : x(val), y(val), z(val) {};
  vec3(float a, float b, float c) : x(a), y(b), z(c) {};
  void print();
  static vec3   cross(vec3 a, vec3 b);
  static vec3   normalize(vec3 a);
  static float  dot(vec3 a, vec3 b);
  vec3 operator + (vec3 const& b);
  vec3 operator - (vec3 const& b);
  vec3 operator * (float const& s);
};

struct vec4 {
  union {
    float ptr[4];
    struct {
      union { float x; float r; };
      union { float y; float g; };
      union { float z; float b; };
      union { float w; float a; };
    };
  };
  vec4() : x(0), y(0), z(0), w(0) {};
  vec4(float val) : x(val), y(val), z(val), w(val) {};
  vec4(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) {};
  vec4(vec3 v, float d) : x(v.x), y(v.y), z(v.z), w(d) {};
  void print();
  static vec4   normalize(vec4 a);
  static float  dot(vec4 a, vec4 b);
  vec4 operator + (vec4 const& b);
  vec4 operator - (vec4 const& b);
  vec4 operator * (float const& s);
};

// 4x4 Matrix

struct mat4 {
  union {
    // Pointer access
    float ptr[16] = { 1,0,0,0,
                      0,1,0,0,
                      0,0,1,0,
                      0,0,0,1};
    // Letter access
    struct {
      float a,b,c,d,
            e,f,g,h,
            i,j,k,l,
            m,n,o,p;
    };
    // Position access
    struct {
      float _00,_01,_02,_03,
            _10,_11,_12,_13,
            _20,_21,_22,_23,
            _30,_31,_32,_33;
    };
  };
  mat4() {};
  mat4(float src[16]) { for(int n=0;n<16;n++) ptr[n]=src[n]; };
  void print();
  static mat4 identity();
  static mat4 mult(mat4 A, mat4 B);
  static mat4 translate(vec3 t);
  static mat4 scale(vec3 s);
  static mat4 rotate(float th, vec3 axis);
  static mat4 perspective(float fovy, float asp, float zNear, float zFar);
  static mat4 look(vec3 eye, vec3 point, vec3 up);
  friend mat4 operator * (mat4 const& a, mat4 const& b) { return mat4::mult(a,b); };
};

struct mat3 {
  union {
    // Pointer access
    float ptr[9] = {1,0,0,
                    0,1,0,
                    0,0,1};
    // Letter access
    struct {
      float a,b,c,
            d,e,f,
            g,h,i;
    };
    // Position access
    struct {
      float _00,_01,_02,
            _10,_11,_12,
            _20,_21,_22;
    };
  };
  mat3() {};
  mat3(mat4 mv) : a(mv.a), b(mv.b), c(mv.c),
                  d(mv.e), e(mv.f), f(mv.g),
                  g(mv.i), h(mv.j), i(mv.k) {};
  mat3(float src[9]) { for(int n=0;n<9;n++) ptr[n]=src[n]; };
  void print();
  static mat3 normal(mat4 modelView);
};

// Constants, now that everything's defined
const vec3 I (1,0,0);
const vec3 J (0,1,0);
const vec3 K (0,0,1);

/*
 * Layer 1: Basics
 * This covers context, shaders, textures, and other things,
 * but maintains general OpenGL flow.
*/

// Enumerations for the below classes
enum UType {U_INT, U_FLOAT, U_VEC2, U_VEC3, U_VEC4, U_MAT3, U_MAT4, U_MTL}; // Uniforms

// Vertex Buffer Object
class VBO {
private:
  GLuint vbo_name = 0;
  GLint pt_count = 0;
public:
  VBO();
  VBO(GLfloat* data, GLsizeiptr dat_arr_size, GLint count);
  VBO(GLfloat* data, GLuint* indices, GLsizeiptr dat_arr_size, GLsizeiptr idx_arr_size, GLint count);
  void loadFromOBJ(std::string filename); // Defined in loadobj.cpp
  void bind();
  void unbind();
  bool verify();
  bool draw(GLenum mode = GL_TRIANGLES);
};

// Texture (24-byte RGB)
class Texture {
private:
  GLuint tex_name = 0;
public:
  Texture(){};
  GLuint getName() { return tex_name; };
  void loadFromBMP(std::string filename);
  Texture(std::string filename) { loadFromBMP(filename); };
  void bind() { glBindTexture(GL_TEXTURE_2D, tex_name); };
  void unbind() { glBindTexture(GL_TEXTURE_2D, 0); };
  bool verify();
};

class Material {
private:
  Texture color;
  Texture normal;
  Texture ambient;
  Texture diffuse;
  Texture specular;
public:
  Material() {};
  void setColorTex(Texture C) { color = C; };
  void setNormalTex(Texture N) { normal = N; };
  void setAmbientTex(Texture A) { ambient = A; };
  void setDiffuseTex(Texture D) { diffuse = D; };
  void setSpecularTex(Texture S) { specular = S; };
  void setFromFolder(std::string folder, std::string basename);
  void bind();
  void unbind();
};

// Shader Program
class Shader {
private:
  GLuint shader_name = 0;
  std::string fn_vertex_shader = "";
  std::string fn_tess_control_shader = "";
  std::string fn_tess_eval_shader = "";
  std::string fn_geometry_shader = "";
  std::string fn_fragment_shader = "";
public:
  Shader();
  GLuint getName() { return shader_name; };
  // Set files
  void setShaderFile(GLenum type, std::string filename);
  // Compile shader
  bool compile();
  // Bind/unbind shader
  void bind();
  void unbind();
  // Verify that *this shader* is active and valid
  bool verify();
  void setUniform(UType type, std::string name, void* val_ptr);
  // Create material-texture bindings
  void setMTL(Material m);
};

// Window
// Mostly just wraps GLFWwindow.
class Window {
private:
  GLFWwindow* win = nullptr;
  vec2 dim = vec2(600, 600);
  std::string name = "Window";
  void* reshape = nullptr;
  void* keyboard = nullptr;
public:
  Window();
  Window(vec2 size, std::string title);
  void setResizeCallback(GLFWwindowsizefun func);
  void setKeyboardCallback(GLFWkeyfun func);
  GLFWwindow* getRaw() { return win; };
  vec2 getSize() { return dim; };
  bool shouldClose() { return glfwWindowShouldClose(win); };
  void destroy() { glfwDestroyWindow(win); };
  void swapBuffers() { glfwSwapBuffers(win); };
};

// Camera
// Tracks position and rotation of view
// Supports yaw->pitch. Roll is a pain and I'm not using it right now.
class Camera {
private:
  vec3 pos = vec3(0,0,5); // Position
  vec3 up = vec3(0,1,0);  // Up
  vec3 fw = vec3(0,0,-1); // Forward
  int yaw = 0; int pitch = 0;
  bool mouse_captured = false;
public:
  Camera() {};
  Camera(vec3 _pos) {};
  void move(vec3 dp, bool useYaw, bool usePitch); // INCREMENT position
  void rotate(int y, int p) { yaw = yaw+y; pitch = pitch+p; }; // INCREMENT rotation
  void setPos(vec3 _pos) { pos = _pos; }; // SET position
  void setRot(int y, int p) { yaw = y; pitch = p; }; // SET rotation
  vec3 getPos() { return pos; };
  mat4 asView(); // Camera as view matrix
};

#endif