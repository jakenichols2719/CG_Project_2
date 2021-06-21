#include "jngl.h"

static vec4 mult (mat4 mA, vec4 vb) {
  vec4 res;
  // Matrix-vector mult
  float* r = res.ptr; float* A = mA.ptr; float* b = vb.ptr;
  for(int n=0; n<16; n+=4) {
    for(int i=n; i<n+4; i++) {
      *r += (*A)*(*b);
      A++; b++;
    }
    r++;
    b-=4;
  }
  return res;
}

// Move camera relative to rotation (-z is forward always)
void Camera::move(vec3 dp, bool useYaw, bool usePitch) {
  // Get horizontal rotation
  mat4 mat_yaw = mat4::rotate(-1*yaw,vec3(0,1,0));
  mat4 mat_pitch = mat4::rotate(-1*pitch,vec3(1,0,0));
  mat4 rot;
  if(useYaw) rot = rot*mat_yaw;
  if(usePitch) rot = rot*mat_pitch;
  vec4 dpv4 (dp.x, dp.y, dp.z, 1);
  vec4 rel;
  rel = mult(rot, dpv4);
  pos = pos + vec3(rel.x, rel.y, rel.z);
}

mat4 Camera::asView() {
  mat4 mat_yaw = mat4::rotate(yaw,vec3(0,1,0));
  mat4 mat_pitch = mat4::rotate(pitch,vec3(1,0,0));
  mat4 mat_pos = mat4::translate(pos*-1);

  return mat_pitch*mat_yaw*mat_pos;
}