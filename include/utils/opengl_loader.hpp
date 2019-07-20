#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#ifdef _WIN32
#include <GL/wglext.h>
#endif
#include <EGL/egl.h>

#include <iostream>
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLGETSTRINGIPROC glGetStringi;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding;
extern PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;


void *loadFunction(const char *name)
{
#ifdef _WIN32
	void *f = (void *)wglGetProcAddress(name);

	if (!f || (f == (void *)0x1) || (f == (void *)0x3)
		|| (f == (void *)-1)) {
		OutputDebugString("\n Cant find ProcAddress in ICD.");
		HMODULE module = LoadLibrary("opengl32.dll");
		f = (void *)GetProcAddress(module, name);

		if (!f)
			return NULL;
	}
	return f;
#elif defined __linux__
    void *f = (void *)eglGetProcAddress(name);
    if (!f) {
        std::cout << "Failed to get load GL Function : " << name << std::endl;
        return NULL;
    }
    return f;
#endif
}

void loadGLFunctions()
{
    glGenBuffers = (PFNGLGENBUFFERSPROC)loadFunction("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)loadFunction("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)loadFunction("glBufferData");
	glGetStringi = (PFNGLGETSTRINGIPROC)loadFunction("glGetStringi");
	glCreateShader = (PFNGLCREATESHADERPROC)loadFunction("glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)loadFunction("glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)loadFunction("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)loadFunction("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)loadFunction("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)loadFunction("glGetShaderInfoLog");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)loadFunction("glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)loadFunction("glAttachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)loadFunction("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)loadFunction("glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)loadFunction("glGetProgramInfoLog");
	glUseProgram = (PFNGLUSEPROGRAMPROC)loadFunction("glUseProgram");
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)loadFunction("glGenVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)loadFunction("glBindVertexArray");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)loadFunction("glVertexAttribPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)loadFunction("glEnableVertexAttribArray");
	glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC)loadFunction("glVertexArrayAttribBinding");
	glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC)loadFunction("glVertexArrayVertexBuffer");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)loadFunction("glGetUniformLocation");
	glUniform4f = (PFNGLUNIFORM4FPROC)loadFunction("glUniform4f");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)loadFunction("glUniformMatrix4fv");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)loadFunction("glGenerateMipmap");
}
