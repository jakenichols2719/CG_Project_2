#include "jngl.h"

void Debug::print(std::string print_msg) {
  std::cout << "DEBUG: ";
  std::cout << print_msg << std::endl;
}

void Debug::warn(std::string warn_msg) {
  std::cout << "WARNING: ";
  std::cout << warn_msg << std::endl;
}

void Debug::err(std::string err_msg) {
  std::cout << "ERROR: ";
  std::cout << err_msg << std::endl;
  std::cout << "Error is fatal. Exiting program" << std::endl;
  exit(0);
}

bool Debug::GLerr(std::string err_msg) {
  GLenum err; bool has_err = false;
  while((err=glGetError()) != GL_NO_ERROR) {
    has_err = true;
    std::cout << gluErrorString(err) << std::endl;
  }
  if(has_err) std::cout << "at " << err_msg << std::endl;
  return has_err;
}

void APIENTRY debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
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