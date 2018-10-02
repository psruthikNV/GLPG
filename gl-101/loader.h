#pragma once

#include <gl\GL.h>
#include <gl\glext.h>
#include <gl\wglext.h>

#pragma comment (lib, "opengl32.lib")
#pragma warning(disable:4996)

PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLGETSTRINGIPROC glGetStringi;

void *loadFunction(const char *name)
{
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
}

int getWglExtensions(HDC hDc)
{	
	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)loadFunction("wglGetExtensionsStringARB");

	if (wglGetExtensionsStringARB == NULL)
		return 0;
	
	const char *extensions = extensions = wglGetExtensionsStringARB(hDc);
	OutputDebugString(extensions);

	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)loadFunction("wglChoosePixelFormatARB");

	if (wglChoosePixelFormatARB == NULL)
		return 0;

	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)loadFunction("wglCreateContextAttribsARB");

	if (wglCreateContextAttribsARB == NULL)
		return 0;

	return 1;
}

void loadGlFunctions(void)
{
	glGenBuffers = (PFNGLGENBUFFERSPROC)loadFunction("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)loadFunction("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)loadFunction("glBufferData");
	glGetStringi = (PFNGLGETSTRINGIPROC)loadFunction("glGetStringi");
}