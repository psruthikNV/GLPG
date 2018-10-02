#include <iostream>
#include <Windows.h>
#include "context.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND createNativeWindow(HINSTANCE *hInstance)
{
	WNDCLASS wc = { 0 };
	HWND hWnd;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = *hInstance;
	wc.lpszClassName = "Class";
	wc.style = CS_OWNDC;

	if (!RegisterClass(&wc)) {
		OutputDebugString("Failed to Register Window Class \n");
	}

	hWnd = CreateWindow(wc.lpszClassName, "Test", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, 1366, 768, 0, 0, *hInstance, 0);

	return hWnd;
}

void loop(HDC *hDc)
{
	glClearColor(0.0, 0.5, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	SwapBuffers(*hDc);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	glContext gC;
	HWND hWnd;

	hWnd = createNativeWindow(&hInstance);

	gC.createGlContext(&hWnd);

	//MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
	//glViewport(0, 0, 640, 480);

	while(1)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			DispatchMessage(&msg);
		loop(&gC.hDc);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	break;

	case WM_CLOSE:
		OutputDebugString("Closing the Window");
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}