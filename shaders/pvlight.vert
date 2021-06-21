#version 400 core

//  Transformation matrices
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ViewMatrix;
uniform mat3 NormalMatrix;

// Light properties
uniform vec4 Ambient;
uniform vec4 Diffuse;
uniform vec4 Specular;
uniform vec4 Position;

//  Vertex attributes (input)
layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec2 Texture;
layout(location = 2) in vec3 Normal;
vec4 Color = vec4(.75);

//  Output to next shader
out VS_FS_INTERFACE {
  out vec4 vCol; // Color
} vs_out;

vec4 phong()
{
   //  P is the vertex coordinate on body
   vec3 P = vec3(ModelViewMatrix * Vertex);
   //  N is the object normal at P
   vec3 N = normalize(NormalMatrix * Normal);
   //  L is the light vector
   vec3 L = normalize(vec3(ViewMatrix * Position) - P);

   //  Emission and ambient color
   vec4 color = Ambient*Color;

   //  Diffuse light intensity is cosine of light and normal vectors
   float Id = dot(L,N);
   if (Id>0.0)
   {
      //  Add diffuse - material color from Color
      color += Id*Diffuse*Color;
      //  R is the reflected light vector R = 2(L.N)N - L
      vec3 R = reflect(-L, N);
      //  V is the view vector (eye at the origin)
      vec3 V = normalize(-P);
      //  Specular is cosine of reflected and view vectors
      //  Assert material specular color is white
      float Is = dot(R,V);
      if (Is>0.0) color += pow(Is,8.0)*Specular;
   }

   //  Return sum of color components
   return color;
}

void main()
{
   mat4 MVP = ProjectionMatrix * ModelViewMatrix;
   gl_Position = MVP * Vertex;
   vs_out.vCol = phong();
}
