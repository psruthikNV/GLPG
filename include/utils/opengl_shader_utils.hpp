#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
extern bool compileShader(GLuint &shaderObject, const char *shaderSource);
extern bool linkShaders(GLuint &programObject);
