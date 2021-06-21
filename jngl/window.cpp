#include "jngl.h"

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
  // Some debug messages are just annoying informational messages
  switch (id)
  {
  case 131185: // glBufferData
      return;
  }

  printf("Message: %s\n", message);
  printf("Source: ");

  switch (source)
  {
  case GL_DEBUG_SOURCE_API:
      printf("API");
      break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      printf("Window System");
      break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
      printf("Shader Compiler");
      break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
      printf("Third Party");
      break;
  case GL_DEBUG_SOURCE_APPLICATION:
      printf("Application");
      break;
  case GL_DEBUG_SOURCE_OTHER:
      printf("Other");
      break;
  }

  printf("\n");
  printf("Type: ");

  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:
      printf("Error");
      break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      printf("Deprecated Behavior");
      break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      printf("Undefined Behavior");
      break;
  case GL_DEBUG_TYPE_PORTABILITY:
      printf("Portability");
      break;
  case GL_DEBUG_TYPE_PERFORMANCE:
      printf("Performance");
      break;
  case GL_DEBUG_TYPE_MARKER:
      printf("Marker");
      break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
      printf("Push Group");
      break;
  case GL_DEBUG_TYPE_POP_GROUP:
      printf("Pop Group");
      break;
  case GL_DEBUG_TYPE_OTHER:
      printf("Other");
      break;
  }

  printf("\n");
  printf("ID: %d\n", id);
  printf("Severity: ");

  switch (severity)
  {
  case GL_DEBUG_SEVERITY_HIGH:
      printf("High");
      break;
  case GL_DEBUG_SEVERITY_MEDIUM:
      printf("Medium");
      break;
  case GL_DEBUG_SEVERITY_LOW:
      printf("Low");
      break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
      printf("Notification");
      break;
  }

  printf("\n\n");
}

Window::Window() {};
Window::Window(vec2 size, std::string title) {
  // Standard window hints
  glfwWindowHint(GLFW_RESIZABLE,1);     //  Window can be resized
  glfwWindowHint(GLFW_DOUBLEBUFFER,1);  //  Window has double buffering
  glfwWindowHint(GLFW_DEPTH_BITS,24);   //  Prefer 24 depth bits
  glfwWindowHint(GLFW_ALPHA_BITS,8);    //  Prefer 8 alpha bits
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,GL_TRUE); // Debug output
#ifdef APPLE_GL4
  // Apple magic. Thanks, Steve!
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif

  // Set variables
  dim = size;
  name = title;

  // Create window with GLFW
  win = glfwCreateWindow(dim.x, dim.y, title.c_str(), NULL, NULL);
  if(!win) {
    Debug::err("Failed to create window.");
  }
  glfwMakeContextCurrent(win);

  // Enable debug output
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(debugCallback, NULL);

  // Enable VSync
  glfwSwapInterval(1);

  // Initalize glew
  #ifdef USEGLEW
  glewInit();
  #endif
}

void Window::setResizeCallback(GLFWwindowsizefun func) {
  glfwSetWindowSizeCallback(win,func);
}

void Window::setKeyboardCallback(GLFWkeyfun func) {
  glfwSetKeyCallback(win,func);
}
