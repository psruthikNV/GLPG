#pragma once

#include <Windows.h>
#include "loader.h"

class glContext
{
public:
	HDC hDc;
	HGLRC hGlrc;
	HWND *hWnd;
	int pixelFormat;
	
	glContext() : hGlrc(NULL), temp_hGlrc(NULL) {}

	void createContext(HWND *);
private:
	HGLRC temp_hGlrc;
};

void glContext::createContext(HWND *hWnd)
{
	this->hWnd = hWnd;
	hDc = GetDC(*this->hWnd);

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,
		8,
		0,
		0,
		0,
		0,0,0
	};
	
	pixelFormat = ChoosePixelFormat(this->hDc, &pfd);
	SetPixelFormat(hDc, pixelFormat, &pfd);
	temp_hGlrc = wglCreateContext(hDc);
	wglMakeCurrent(hDc, temp_hGlrc);
	int pf[10];
	UINT pfn;
	const int attribList[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0,
	};
	int pfs;

	if (!getWglExtensions(hDc))
		hGlrc = temp_hGlrc;
	else {

		pfs = wglChoosePixelFormatARB(hDc, attribList, NULL, 5, pf, &pfn);

		if (pfs == FALSE)
			OutputDebugString("Failed to set pixel format");

		if (SetPixelFormat(hDc, pf[0], NULL) == FALSE)
			OutputDebugString("FAIL");
	}
	
	const int contextAttrib[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 5,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	hGlrc = wglCreateContextAttribsARB(hDc, 0, contextAttrib);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(temp_hGlrc);
	wglMakeCurrent(hDc, hGlrc);
}