#pragma once
#include "utils/opengl_context.hpp"

extern bool compileShader(int shaderObject, const char *shaderSource);
extern bool linkShaders(int programObject);
