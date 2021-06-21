#version 400 core

//  Transformation matrices
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ViewMatrix;
uniform mat3 NormalMatrix;

// Light properties
uniform vec3 light_pos;
uniform vec4 light_col = vec4(1);
uniform float light_brightness = 1.0;

uniform sampler2D tex_col;
uniform sampler2D tex_nrm;
uniform sampler2D tex_amb;
uniform sampler2D tex_diff;
uniform sampler2D tex_spec;
uniform samplerCube depthMap; // oh boy oh please work oh man
uniform float far;

uniform float shininess = 8.0;
uniform float spec_intensity = 1.0;

//  Vertex attributes (input)
vec4 Color = vec4(.75);

//  Input from previous shader
in GS_FS_INTERFACE {
  vec4 vPos;
  vec2 vTex;
  vec3 vNrm;
} fs_in;

//  Fragment color
layout (location=0) out vec4 Fragcolor;

float shadow(vec3 model_pos) {
  vec3 fragToLight = model_pos-light_pos;
  float closestDepth = texture(depthMap, fragToLight).r * far;
  float currentDepth = length(fragToLight);
  float bias = 0.08;
  float shadow = currentDepth-bias < closestDepth ? 1.0 : 0.0;
  return shadow;
}

vec3 surfaceNormal() {
  // Vertex normals
  vec3 vn1 = vec3(0,1,0);
  vec3 vn2 = fs_in.vNrm;
  vn2.x *= -1;
  // Axis and degree of rotation
  vec3 axis = normalize(cross(vn1, vn2));
  if(axis.length() <= .00005) return vn2;
  float deg = acos(dot(vn1, vn2));
  // Matrix for rotation
  float s = sin(deg);
  float c = cos(deg);
  mat3 R = mat3 (
    (1-c)*axis.x*axis.x+c     , (1-c)*axis.x*axis.y-axis.z*s , (1-c)*axis.z*axis.x+axis.y*s ,
    (1-c)*axis.x*axis.y+axis.z*s , (1-c)*axis.y*axis.y+c     , (1-c)*axis.y*axis.z+axis.x*s ,
    (1-c)*axis.z*axis.x-axis.y*s , (1-c)*axis.y*axis.z-axis.x*s , (1-c)*axis.z*axis.z+c
  );
  // Normal map normal
  vec3 ntex = (vec3(texture(tex_nrm, fs_in.vTex))-.5)*2;
  vec3 ntex_swapped = vec3(ntex.x, ntex.z, ntex.y);
  vec3 snrm = R*ntex_swapped;
  if(dot(vn1,vn2)>=.95) snrm = ntex_swapped;
  return snrm;
}

vec4 phong()
{
   //  P is the vertex coordinate on body
   vec3 P = vec3(ModelViewMatrix * fs_in.vPos);
   //  N is the object normal at P
   // Blender uses Z-up, so have to swap b-g in normal map for Y-up
   vec4 nrmtex = vec4((vec3(texture(tex_nrm, fs_in.vTex))-.5)*2, 1);
   //vec3 normal = fs_in.vNrm;
   vec3 N = normalize(NormalMatrix * surfaceNormal());
   //  L is the light vector
   vec3 L = normalize(vec3(ViewMatrix * vec4(light_pos,1)) - P);

   //  Emission and ambient color
   vec4 amb = texture(tex_amb, fs_in.vTex).r * light_col * light_brightness;
   vec4 color = amb*texture(tex_col, fs_in.vTex);

   //  Diffuse light intensity is cosine of light and normal vectors
   float Id = dot(L,N);
   if (Id>0.0)
   {
      //  Add diffuse - material color from Color
      vec4 diff = Id*texture(tex_diff, fs_in.vTex).r*light_col*light_brightness;
      color += Id*texture(tex_diff, fs_in.vTex)*color*shadow(vec3(fs_in.vPos));
      //  R is the reflected light vector R = 2(L.N)N - L
      vec3 R = reflect(-L, N);
      //  V is the view vector (eye at the origin)
      vec3 V = normalize(-P);
      //  Specular is cosine of reflected and view vectors
      //  Assert material specular color is white
      float Is = dot(R,V);
      if (Is>0.0) {
        vec4 spec = pow(Is,shininess)*texture(tex_spec,fs_in.vTex).r*light_col*light_brightness;
        color += spec*spec_intensity*shadow(vec3(fs_in.vPos));
      }
   }
   //  Return sum of color components
   return color;
}

void main()
{
   Fragcolor = phong();
}
