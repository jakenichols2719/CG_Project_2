/*
 * mat.cpp
 * Lots of matrix math, using the OpenGL red book as a primary reference,
 * as well as previous classwork that involved examples of matrix math.
*/

#include "jngl.h"
#include <stdio.h> // need formatted output

void mat4::print() {
  printf("mat4:\n");
  // print rows
  for(int n=0; n<16; n+=4) {
    printf("%10.6f %10.6f %10.6f %10.6f\n", ptr[n], ptr[n+1], ptr[n+2], ptr[n+3]);
  }
}

mat4 mat4::mult(mat4 A, mat4 B) {
  float* Ap = A.ptr; float* Bp = B.ptr;
  float out[16];
  for(int rA = 0; rA<4; rA++) {
    vec4 rAv = vec4(Ap[4*rA+0],Ap[4*rA+1],Ap[4*rA+2],Ap[4*rA+3]);
    for(int cB = 0; cB<4; cB++) {
      vec4 cBv = vec4(Bp[cB+0],Bp[cB+4],Bp[cB+8],Bp[cB+12]);
      out[4*rA+cB] = vec4::dot(rAv,cBv);
    }
  }
  return mat4(out);
}

mat4 mat4::identity() { return mat4(); };

mat4 mat4::rotate(float th, vec3 a) {
  a = vec3::normalize(a);
  float s = sin(th*M_PI/180);
  float c = cos(th*M_PI/180);
  float R[16] = {
    (1-c)*a.x*a.x+c     , (1-c)*a.x*a.y-a.z*s , (1-c)*a.z*a.x+a.y*s , 0 ,
    (1-c)*a.x*a.y+a.z*s , (1-c)*a.y*a.y+c     , (1-c)*a.y*a.z+a.x*s , 0 ,
    (1-c)*a.z*a.x-a.y*s , (1-c)*a.y*a.z-a.x*s , (1-c)*a.z*a.z+c     , 0 ,
              0         ,         0           ,           0         , 1
  };
  return mat4(R);
}

mat4 mat4::translate(vec3 t) {
  mat4 T;
  T._03 = t.x;
  T._13 = t.y;
  T._23 = t.z;
  return T;
}

mat4 mat4::scale(vec3 s) {
  mat4 S;
  S._00 = s.x;
  S._11 = s.y;
  S._22 = s.z;
  return S;
}

mat4 mat4::perspective(float fovy, float asp, float zNear, float zFar) {
  mat4 P;
  float cot = cos(fovy/2*M_PI/180)/sin(fovy/2*M_PI/180);

  P._00 = cot/asp;
  P._11 = cot;
  P._22 = -(zFar+zNear)/(zFar-zNear);
  P._23 = -2*zFar*zNear/(zFar-zNear);
  P._32 = -1;
  P._33 = 0;

  return P;
}

mat4 mat4::look(vec3 eye, vec3 point, vec3 up) {
  vec3 fw = vec3::normalize(point-eye);       // Forward vector
  vec3 rt = vec3::cross(fw,up);               // Right vector
       rt = vec3::normalize(rt);
       up = vec3::cross(rt,fw);               // Up vector (recalculated)

  float L[16] = {
    rt.x, rt.y, rt.z, 0,
    up.x, up.y, up.z, 0,
    -fw.x, -fw.y, -fw.z, 0,
      0 ,   0 ,   0 , 1
  };
  return mat4(L)*mat4::translate(eye*-1);
}

void mat3::print() {
  printf("mat3:\n");
  // print rows
  for(int n=0; n<9; n+=3) {
    printf("%10.6f %10.6f %10.6f\n", ptr[n], ptr[n+1], ptr[n+2]);
  }
}

mat3 mat3::normal(mat4 mv) {

  float det = mv.a*(mv.f*mv.k-mv.g*mv.j)
            - mv.b*(mv.e*mv.k-mv.g*mv.i)
            + mv.c*(mv.e*mv.j-mv.f*mv.i);
  if(det*det<1e-25) {
    std::cout << det << std::endl;
  }
  mat3 in;
  // Inverse using Cramer's Rule
  in.a = (mv.f*mv.k-mv.g*mv.j)/det;
  in.b = (mv.e*mv.k-mv.g*mv.i)/det*-1;
  in.c = (mv.e*mv.j-mv.f*mv.i)/det;
  in.d = (mv.b*mv.k-mv.c*mv.j)/det*-1;
  in.e = (mv.a*mv.k-mv.c*mv.i)/det;
  in.f = (mv.a*mv.j-mv.b*mv.i)/det*-1;
  in.g = (mv.b*mv.g-mv.c*mv.f)/det;
  in.h = (mv.a*mv.g-mv.c*mv.e)/det*-1;
  in.i = (mv.a*mv.f-mv.b*mv.e)/det;
  return in;
}