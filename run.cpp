#include "jngl.h"

// Window and properties
Window window; vec2 winsize (600); float asp=1.0; float dim=3;
float angle_h = 0; float angle_v = 0;
// Shader
Shader mtl, mtl_nnrm, mtlw, depth, texlight, white, fbquad;
// Objects
VBO plane, cube;
/*
// Fireplace (no normal maps, for now)
VBO obj_mantle, obj_deco, obj_grate, obj_fire;
Material mtl_mantle, mtl_deco, mtl_grate, mtl_fire;
*/
VBO obj_floor, obj_ceil, obj_trim, obj_wallpaper, obj_chairs,
    obj_desk, obj_windows, obj_book,
    obj_lamps, obj_lamptops, obj_curtains, obj_cord;
// Materials
Material mtl_floor, mtl_ceil, mtl_trim, mtl_wallpaper, mtl_chairs,
         mtl_desk, mtl_windows, mtl_book,
         mtl_lamps, mtl_lamptops, mtl_curtains, mtl_cord;
Texture  bump_wallpaper;
float TLI = 200; float TLO = 200;
/*
Material crate, logs;
*/
// Camera
Camera camera;
// Lighting
vec3 light_pos;
int light_angle = 0;
vec4 light_col = vec4(1.0,0.87,0.70,1);
float light_brightness = 0.8;
bool light_moving = true;
bool light_circle = true;
// Depth map
const GLuint SHADOW_WIDTH=2048,SHADOW_HEIGHT=2048;
GLuint depthBuffer;
GLuint depthCubeMap;
mat4 sT[6]; // Shadow transformations for each face, indexed

float dsin(float th) { return sin(M_PI*th/180); };
float dcos(float th) { return cos(M_PI*th/180); };

// Shamelessly stolen FPS code
static int fps=0,sec0=0,count=0,fplock=false;
int FramesPerSecond(void)
{
   int sec = glfwGetTime();
   if (sec!=sec0)
   {
      sec0 = sec;
      fps = count;
      count=0;
   }
   count++;
   return fps;
}

void reshape(GLFWwindow* window,int width,int height)
{
  //  Get framebuffer dimensions (makes Apple work right)
  glfwGetFramebufferSize(window,&width,&height);
  // Set window size properties
  winsize.x = width;
  winsize.y = height;
  asp = (float)width/height;
  //  Set the viewport to the entire window
  //glViewport(0,0, width,height);
}

void key(GLFWwindow* window,int key,int scancode,int action,int mods) {
  if(action==GLFW_RELEASE) return;

  else if(key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window,1);
  else if(key == GLFW_KEY_LEFT) angle_h-=5;
  else if(key == GLFW_KEY_RIGHT) angle_h+=5;
  else if(key == GLFW_KEY_DOWN) angle_v-=5;
  else if(key == GLFW_KEY_UP) angle_v+=5;
  else if(key == GLFW_KEY_W) camera.move(vec3(0,0,-1),true,false);
  else if(key == GLFW_KEY_S) camera.move(vec3(0,0,+1),true,false);
  else if(key == GLFW_KEY_A) camera.move(vec3(-1,0,0),true,false);
  else if(key == GLFW_KEY_D) camera.move(vec3(+1,0,0),true,false);
  else if(key == GLFW_KEY_L) light_moving = !light_moving;
  else if(key == GLFW_KEY_P) {
    light_moving = false;
    light_circle = false;
    light_pos = camera.getPos();
  }
  else if(key == GLFW_KEY_O) {
    light_moving = true;
    light_circle = true;
  }
  else if(key == GLFW_KEY_EQUAL) light_angle++;
  else if(key == GLFW_KEY_MINUS) light_angle--;
}

static void drawAt(VBO object, Shader shader, mat4 T, mat4 R, mat4 S, mat4 View, mat4 Perspective, GLenum type=GL_QUADS) {
  object.bind();
  shader.bind();
  // Set attributes
  // Set attributes
  glVertexAttribPointer(0,4,GL_FLOAT,0,36,(void*) 0); // Vertex
  glVertexAttribPointer(1,3,GL_FLOAT,0,36,(void*)16); // Texture
  glVertexAttribPointer(2,4,GL_FLOAT,0,36,(void*)24); // Normal
  glEnableVertexAttribArray(0); // Vertex
  glEnableVertexAttribArray(1); // Texture
  glEnableVertexAttribArray(2); // Normal
  mat4 M = T*R*S;
  mat4 MV = View*M;
  mat3 N = mat3::normal(MV);
  //N.print();

  shader.setUniform(U_MAT4, "ProjectionMatrix", Perspective.ptr);
  shader.setUniform(U_MAT4, "ViewMatrix", View.ptr);
  shader.setUniform(U_MAT4, "ModelViewMatrix", MV.ptr);
  shader.setUniform(U_MAT3, "NormalMatrix", N.ptr);
  shader.setUniform(U_MAT4, "ModelMatrix", M.ptr);
  object.draw(type);

  // Disable attributes
  // Disable arrays
  glDisableVertexAttribArray(0); // Vertex
  glDisableVertexAttribArray(1); // Texture
  glDisableVertexAttribArray(2); // Normal

  object.unbind();
  shader.unbind();
}

static void drawAtForDepth(VBO object, mat4 T, mat4 R, mat4 S, GLenum type=GL_QUADS) {
  Shader shader = depth;
  object.bind();
  shader.bind();
  // Set attributes
  glVertexAttribPointer(0,4,GL_FLOAT,0,36,(void*) 0); // Vertex
  glEnableVertexAttribArray(0); // Vertex
  mat4 M = T*R*S;
  // Set uniforms for depth shader
  shader.setUniform(U_MAT4, "ModelMatrix", M.ptr);
  object.draw(type);

  // Disable attributes
  glDisableVertexAttribArray(0); // Vertex

  object.unbind();
  shader.unbind();
}

static void createDepthBuffer() {
  // Generate depth framebuffer
  glGenFramebuffers(1,&depthBuffer);
  // Generate depth cubemap
  glGenTextures(1,&depthCubeMap);
  // Define the texture as a depth component cubemap with float format
  // Iterate through all 6 faces
  glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
  for(GLuint n=0; n<6; n++) {
    // Define the properties of each face
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+n, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }

  // Attach the texture as a depth component to the framebuffer
  // Since we're using a geometry shader, we don't need to swap face each render
  glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  // Unbind framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  Debug::GLerr("createDepthBuffer");
}

static void shadowPass() {
  // Set up shadow projection
  float shadowAsp = SHADOW_WIDTH/SHADOW_HEIGHT;
  float near = 1.0;
  float far = dim*16;
  // theta=90 so projects outward as cube
  mat4 sP = mat4::perspective(90, shadowAsp, near, far);
  // Shadow transform list, 1 for each face)
  sT[0] = (sP*mat4::look(light_pos, light_pos+vec3(1,0,0), vec3(0,-1,0)));
  sT[1] = (sP*mat4::look(light_pos, light_pos+vec3(-1,0,0), vec3(0,-1,0)));
  sT[2] = (sP*mat4::look(light_pos, light_pos+vec3(0,1,0), vec3(0,0, 1)));
  sT[3] = (sP*mat4::look(light_pos, light_pos+vec3(0,-1,0), vec3(0,0,-1)));
  sT[4] = (sP*mat4::look(light_pos, light_pos+vec3(0,0,1), vec3(0,-1,0)));
  sT[5] = (sP*mat4::look(light_pos, light_pos+vec3(0,0,-1), vec3(0,-1,0)));
  sT[0].print();

  // Setup depth buffer
  glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer);
  glClear(GL_DEPTH_BUFFER_BIT);
  // Use depth shader
  depth.bind();
  // Send uniforms to depth shader
  for(int n=0; n<6; n++) {
    std::string uName = "shadowMatrices["+std::to_string(n)+"]";
    depth.setUniform(U_MAT4, uName, sT[n].ptr);
  }
  depth.setUniform(U_VEC3, "light_pos", light_pos.ptr);
  depth.setUniform(U_FLOAT, "far", &far);

  // Draw all for depth
  drawAtForDepth(obj_wallpaper,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         GL_QUADS);
  // Wallpaper
  drawAtForDepth(obj_floor,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         GL_QUADS);
  // Ceiling
  drawAtForDepth(obj_floor,
         mat4::translate(vec3(4,13,0)),
         mat4::rotate(180,vec3(0,0,1)),
         mat4::scale(vec3(1,1,1)),
         GL_QUADS);
  // Trim
  drawAtForDepth(obj_trim,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         GL_QUADS);
  // Chairs
  drawAtForDepth(obj_chairs,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         GL_TRIANGLES);
  // Desk
  drawAtForDepth(obj_desk,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         GL_TRIANGLES);
  // Windows
  drawAtForDepth(obj_windows,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         GL_TRIANGLES);

  // Book
  drawAtForDepth(obj_book,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         GL_TRIANGLES);

  // Lamps
  drawAtForDepth(obj_lamps,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         GL_TRIANGLES);

  // Curtains
  drawAtForDepth(obj_curtains,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         GL_TRIANGLES);

  // Cords
  drawAtForDepth(obj_cord,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         GL_TRIANGLES);

  drawAtForDepth(obj_lamptops,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         GL_TRIANGLES);

  // Unbind framebuffer, for safety
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void scenePass() {
  glViewport(0,0,winsize.x, winsize.y);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  // Eye position
  camera.setRot(angle_h, angle_v);
  // Create perspective, view matrices
  mat4 P = mat4::perspective(57, asp, dim/16, dim*16);
  //mat4 V = camera.asView();
  //vec3 eye = vec3(10*dsin(angle_h)*dcos(angle_v),10*dsin(angle_v),10*dcos(angle_h)*dcos(angle_v));
  mat4 V = camera.asView();//mat4::look(eye, vec3(0,0,0), vec3(0,1,0));

  // Room scene
  mtlw.bind();
  mtlw.setUniform(U_VEC3, "light_pos", light_pos.ptr);
  mtlw.setUniform(U_VEC4, "light_col", light_col.ptr);
  mtlw.setUniform(U_FLOAT,"light_brightness", &light_brightness);
  mtlw.setMTL(mtl_wallpaper);
  //GLint sampler_id = glGetUniformLocation(mtlw.getName(), "tex_bump");
  //glUniform1i(sampler_id, 6);
  glActiveTexture(GL_TEXTURE6); bump_wallpaper.bind();
  glUniform1i(glGetUniformLocation(mtlw.getName(),"tex_bump"),6);
  glUniform1f(glGetUniformLocation(mtlw.getName(),"shininess"),2);
  glUniform1f(glGetUniformLocation(mtlw.getName(),"spec_intensity"),0.75);
  glUniform1f(glGetUniformLocation(mtlw.getName(),"far"),dim*16);
  mtlw.setUniform(U_FLOAT, "TLI", &TLI);
  mtlw.setUniform(U_FLOAT, "TLO", &TLO);
  glPatchParameteri(GL_PATCH_VERTICES,4);
  drawAt(obj_wallpaper, mtlw,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         V, P, GL_PATCHES);
  // Draw again to cover cracks. Kind of cheating. Too bad!
  drawAt(obj_wallpaper, mtlw,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1.05,1.05,1.05)),
         V, P, GL_PATCHES);

  mtl.bind();
  mtl.setUniform(U_VEC3, "light_pos", light_pos.ptr);
  mtl.setUniform(U_VEC4, "light_col", light_col.ptr);
  mtl.setUniform(U_FLOAT,"light_brightness", &light_brightness);
  glUniform1f(glGetUniformLocation(mtl.getName(),"far"),dim*16);

  // Wallpaper
  mtl.setMTL(mtl_floor);
  //std::cout << "Hi!" << std::endl;
  glUniform1f(glGetUniformLocation(mtl.getName(),"shininess"),4.0);
  glUniform1f(glGetUniformLocation(mtl.getName(),"spec_intensity"),0.5);
  drawAt(obj_floor, mtl,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         V, P);
  // Ceiling
  mtl.setMTL(mtl_ceil);
  glUniform1f(glGetUniformLocation(mtl.getName(),"shininess"),4.0);
  glUniform1f(glGetUniformLocation(mtl.getName(),"spec_intensity"),0.5);
  drawAt(obj_floor, mtl,
         mat4::translate(vec3(4,13,0)),
         mat4::rotate(180,vec3(0,0,1)),
         mat4::scale(vec3(1,1,1)),
         V, P);
  // Trim
  mtl.setMTL(mtl_trim);
  glUniform1f(glGetUniformLocation(mtl.getName(),"shininess"),16.0);
  glUniform1f(glGetUniformLocation(mtl.getName(),"spec_intensity"),1.0);
  drawAt(obj_trim, mtl,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         V, P);
  // Chairs
  mtl.setMTL(mtl_chairs);
  glUniform1f(glGetUniformLocation(mtl.getName(),"shininess"),2.0);
  glUniform1f(glGetUniformLocation(mtl.getName(),"spec_intensity"),1.0);
  drawAt(obj_chairs, mtl,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         V, P, GL_TRIANGLES);
  // Desk
  mtl.setMTL(mtl_desk);
  glUniform1f(glGetUniformLocation(mtl.getName(),"shininess"),16.0);
  glUniform1f(glGetUniformLocation(mtl.getName(),"spec_intensity"),1.0);
  drawAt(obj_desk, mtl,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         V, P, GL_TRIANGLES);
  // Windows

  mtl.setMTL(mtl_windows);
  glUniform1f(glGetUniformLocation(mtl.getName(),"shininess"),16.0);
  glUniform1f(glGetUniformLocation(mtl.getName(),"spec_intensity"),1.0);
  drawAt(obj_windows, mtl,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         V, P, GL_TRIANGLES);

  // Book
  mtl.setMTL(mtl_book);
  glUniform1f(glGetUniformLocation(mtl.getName(),"shininess"),32.0);
  glUniform1f(glGetUniformLocation(mtl.getName(),"spec_intensity"),1.0);
  drawAt(obj_book, mtl,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         V, P, GL_TRIANGLES);

  // Lamps
  mtl.setMTL(mtl_lamps);
  glUniform1f(glGetUniformLocation(mtl.getName(),"shininess"),32.0);
  glUniform1f(glGetUniformLocation(mtl.getName(),"spec_intensity"),1.4);
  drawAt(obj_lamps, mtl,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         V, P, GL_TRIANGLES);

  // Curtains
  mtl.setMTL(mtl_curtains);
  glUniform1f(glGetUniformLocation(mtl.getName(),"shininess"),2.0);
  glUniform1f(glGetUniformLocation(mtl.getName(),"spec_intensity"),0.4);
  drawAt(obj_curtains, mtl,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         V, P, GL_TRIANGLES);

  // Cords
  mtl.setMTL(mtl_cord);
  glUniform1f(glGetUniformLocation(mtl.getName(),"shininess"),32.0);
  glUniform1f(glGetUniformLocation(mtl.getName(),"spec_intensity"),1.0);
  drawAt(obj_cord, mtl,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         V, P, GL_TRIANGLES);

  mtl.setMTL(mtl_lamptops);
  glUniform1f(glGetUniformLocation(mtl.getName(),"shininess"),2.0);
  glUniform1f(glGetUniformLocation(mtl.getName(),"spec_intensity"),1.0);
  drawAt(obj_lamptops, mtl,
         mat4::translate(vec3(0,0,0)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(1,1,1)),
         V, P, GL_TRIANGLES);



  // Draw light
  white.bind();
  white.setUniform(U_VEC4, "col", light_col.ptr);
  drawAt(cube, white,
         mat4::translate(vec3(light_pos.x,light_pos.y,light_pos.z)),
         mat4::rotate(0,vec3(0,1,0)),
         mat4::scale(vec3(.1,.1,.1)),
         V, P);
  white.unbind();

  // Display content
  Debug::GLerr("Scene Pass");
}

void display() {
  // Alter light position
  if(light_moving) light_angle++;
  if(light_circle) light_pos = vec3(7.0*dsin(light_angle),7.0,7.0*dcos(light_angle));
  // Draw to shadow map
  shadowPass();
  glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
  glActiveTexture(GL_TEXTURE0);
  // Draw to scene
  scenePass();
  glFlush();
  window.swapBuffers();
}

int main(int argc, char* argv[]) {
  glfwInit();
  // Create window
  window = Window(winsize, "Project: James Nichols");
  window.setResizeCallback(&reshape);
  window.setKeyboardCallback(&key);
  // Create shaders
  texlight.setShaderFile(GL_VERTEX_SHADER,"shaders/texlight.vert");
  texlight.setShaderFile(GL_FRAGMENT_SHADER,"shaders/texlight.frag");
  texlight.compile();
  white.setShaderFile(GL_VERTEX_SHADER,"shaders/white.vert");
  white.setShaderFile(GL_FRAGMENT_SHADER, "shaders/white.frag");
  white.compile();
  mtl.setShaderFile(GL_VERTEX_SHADER,"shaders/mtl.vert");
  mtl.setShaderFile(GL_FRAGMENT_SHADER,"shaders/mtl.frag");
  mtl.compile();
  // Material with bump map for quads (just wall, in this case)
  mtlw.setShaderFile(GL_VERTEX_SHADER,"shaders/mtl_wall.vert");
  mtlw.setShaderFile(GL_TESS_CONTROL_SHADER,"shaders/mtl_wall.tcs");
  mtlw.setShaderFile(GL_TESS_EVALUATION_SHADER,"shaders/mtl_wall.tes");
  mtlw.setShaderFile(GL_GEOMETRY_SHADER,"shaders/mtl_wall.geom");
  mtlw.setShaderFile(GL_FRAGMENT_SHADER,"shaders/mtl_wall.frag");
  mtlw.compile();
  // Shader to render depth
  depth.setShaderFile(GL_VERTEX_SHADER,"shaders/depth.vert");
  depth.setShaderFile(GL_GEOMETRY_SHADER,"shaders/depth.geom");
  depth.setShaderFile(GL_FRAGMENT_SHADER,"shaders/depth.frag");
  depth.compile();
  // Load objects
  plane.loadFromOBJ("models/plane.obj");
  cube.loadFromOBJ("models/cube.obj");
  obj_floor.loadFromOBJ("models/floor.obj");
  mtl_floor.setFromFolder("materials","floor");
  obj_ceil.loadFromOBJ("models/ceiling.obj");
  mtl_ceil.setFromFolder("materials","floor");
  obj_trim.loadFromOBJ("models/trim.obj");
  mtl_trim.setFromFolder("materials","trim");
  obj_wallpaper.loadFromOBJ("models/wallpaper.obj");
  mtl_wallpaper.setFromFolder("materials","wallpaper");
  bump_wallpaper.loadFromBMP("materials/wallpaper_bump.bmp");
  obj_chairs.loadFromOBJ("models/chairs.obj");
  mtl_chairs.setFromFolder("materials","chairs");
  obj_desk.loadFromOBJ("models/desk.obj");
  mtl_desk.setFromFolder("materials","desk");
  obj_windows.loadFromOBJ("models/windows.obj");
  mtl_windows.setFromFolder("materials","windows");
  obj_book.loadFromOBJ("models/book.obj");
  mtl_book.setFromFolder("materials","book");
  obj_lamps.loadFromOBJ("models/lamps.obj");
  mtl_lamps.setFromFolder("materials","lamps");
  obj_lamptops.loadFromOBJ("models/lamptops.obj");
  mtl_lamptops.setFromFolder("materials","lamptops");
  obj_curtains.loadFromOBJ("models/curtains.obj");
  mtl_curtains.setFromFolder("materials","curtains");
  obj_cord.loadFromOBJ("models/cord.obj");
  mtl_cord.setFromFolder("materials","cord");

  // Setup depth buffer/cubemap
  createDepthBuffer();
  // Setup camera
  camera.setPos(vec3(0,8,8));
  // Main loop
  while(!window.shouldClose()) {
    display();
    glfwPollEvents();
    int time = (int)glfwGetTime();
    if((time+1)%10==0) {
      if(!fplock) {
        std::cout << fps << std::endl;
        fplock = true;
      }
    } else {
      fplock = false;
    }
    FramesPerSecond();
  }
  // Close GLFW
  window.destroy();
  glfwTerminate();
  return 0;
}