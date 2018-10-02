/*
 *Info about Window's windows.
 *Each window is associated with a window class.
 *One window class can have multiple windows. 
 *All these windows have to use the WindowProc that was used 
 *during class registration. Window classes are identified
 *by class names.
 *All events are passed to the Window Procedure.
*/

#include <iostream>
#include <Windows.h>
#include "context.h"

#pragma comment (lib, "opengl32.lib")
#pragma warning(disable:4996)

void loop(HDC *hDc)
{
	glClearColor(0.0, 0.5, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	SwapBuffers(*hDc);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	HWND hWnd;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	//wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = "Class";
	wc.style = CS_OWNDC;

	glContext gC;

	if (!RegisterClass(&wc)) {
		OutputDebugString("Failed to Register Window Class \n");
		return -1;
	}

	hWnd = CreateWindow(wc.lpszClassName, "Test", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, 1366, 768, 0, 0, hInstance, 0);

	ShowWindow(hWnd, SW_SHOW);

	gC.createContext(&hWnd);

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