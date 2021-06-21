#include "jngl.h"

//====VEC2====
void vec2::print() {
  using namespace std;
  cout << "vec2: (";
  cout << x << ", " << y;
  cout << ")" << endl;
}

vec2 vec2::normalize(vec2 a) {
  float scl = sqrt(pow(a.x,2) + pow(a.y,2));
  return a*(1/scl);
}

float vec2::dot(vec2 a, vec2 b) {
  return a.x*b.x + a.y*b.y;
}

vec2 vec2::operator + (vec2 const& b)   { return vec2(x+b.x,y+b.y); };
vec2 vec2::operator - (vec2 const& b)   { return vec2(x-b.x,y-b.y); };
vec2 vec2::operator * (float const& s)  { return vec2(x*s,y*s); };

//====VEC3====
void vec3::print() {
  using namespace std;
  cout << "vec3: " << "(";
  cout << x << ", " << y << ", " << z;
  cout << ")" << endl;
}

vec3 vec3::cross(vec3 a, vec3 b) {
  return vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

vec3 vec3::normalize(vec3 a) {
  float scl = sqrt(pow(a.x,2) + pow(a.y,2) + pow(a.z,2));
  return a*(1/scl);
}

float vec3::dot(vec3 a, vec3 b) {
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

vec3 vec3::operator + (vec3 const& b)   { return vec3(x+b.x,y+b.y,z+b.z); };
vec3 vec3::operator - (vec3 const& b)   { return vec3(x-b.x,y-b.y,z-b.z); };
vec3 vec3::operator * (float const& s)  { return vec3(x*s,y*s,z*s); };

//====VEC4====
void vec4::print() {
  using namespace std;
  cout << "vec4: (";
  cout << x << ", " << y << ", " << z << ", " << w;
  cout << ")" << endl;
}

vec4 vec4::normalize(vec4 a) {
  float scl = sqrt(pow(a.x,2) + pow(a.y,2) + pow(a.z,2) + pow(a.w,2));
  return a*(1/scl);
}

float vec4::dot(vec4 a, vec4 b) {
  return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

vec4 vec4::operator + (vec4 const& b)   { return vec4(x+b.x,y+b.y,z+b.z,w+b.w); };
vec4 vec4::operator - (vec4 const& b)   { return vec4(x-b.x,y-b.y,z-b.z,w-b.w); };
vec4 vec4::operator * (float const& s)  { return vec4(x*s,y*s,z*s,w*s); };


