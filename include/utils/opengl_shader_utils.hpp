#pragma once
#include "opengl_context.hpp"
#include <string>

extern bool compileShader(int shaderObject, const char *shaderSource);
extern bool linkShaders(int programObject);
