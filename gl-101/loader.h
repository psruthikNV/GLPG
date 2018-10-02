#pragma once

#include <gl\GL.h>
#include <gl\glext.h>
#include <gl\wglext.h>

#pragma comment (lib, "opengl32.lib")
#pragma warning(disable:4996)

long int wglExtensions = 0;

#define WGL_ARB_PIXEL_FORMAT (1 << 0)
#define WGL_ARB_PIXEL_FORMAT_FLOAT (1 << 1)
#define WGL_ARB_render_texture (1 << 2)
#define WGL_ARB_robustness_application_isolation (1 << 3)
#define WGL_ARB_robustness_share_group_isolation (1 << 4)
#define WGL_3DFX_multisample (1 << 5)
#define WGL_ARB_EXTENSIONS_STRING (1 << 6)
// Function prototypes

/*
* WGL_Extensions supported :
* WGL_EXT_DEPTH_FLOAT
* WGL_ARB_BUFFER_REGION
* WGL_ARB_EXTENSIONS_STRING
* WGL_ARB_MAKE_CURRENT_READ
* WGL_ARB_PIXEL_FORMAT
* WGL_ARB_PBUFFER
* WGL_EXT_EXTENSIONS_STRING
* WGL_GL_EXT_SWAP_CONTROL
* WGL_ARB_MULTISAMPLE
* WGL_ARB_PIXEL_FORMAT_FLOAT
* WGL_ARB_FRAMEBUFFER_SRGB
* WGL_ARB_CREATE_CONTEXT
* WGL_ARB_CREATE_CONTEXT_PROFILE
* WGL_EXT_PIXEL_FORMAT_PACKED_FLOAT
* WGL_EXT_CREATE_CONTEXT_ES_PROFILE
* WGL_EXT_CREATE_CONTEXT_ES2_PROFILE
* WGL_NV_DX_INTEROP
* WGL_WGL_INTEL_CL_SHARING
* WGL_NV_DX_INTEROP2
* WGL_ARB_CREATE_CONTEXT_ROBUSTNESS
* WGL_ARB_CONTEXT_FLUSH_CONTROL
*/

PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
PFNGLGETSTRINGIPROC glGetStringi;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;


/*
void bitset(const char *name)
{
	
}
*/

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

	wglExtensions |= WGL_ARB_EXTENSIONS_STRING;
	
	const char *extensions = extensions = wglGetExtensionsStringARB(hDc);
	OutputDebugString(extensions);

	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)loadFunction("wglChoosePixelFormatARB");

	if (wglChoosePixelFormatARB == NULL)
		return 0;

	wglExtensions |= WGL_ARB_PIXEL_FORMAT;

	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)loadFunction("wglCreateContextAttribsARB");

	if (wglCreateContextAttribsARB == NULL)
		OutputDebugString("Context Creation not supported");

	return 1;
}