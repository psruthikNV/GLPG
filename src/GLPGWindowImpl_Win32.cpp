#include "internal/GLPGWindowImpl_Win32.hpp"

namespace GLPG {

    LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                                LPARAM lParam) {
        switch (message) {
            case WM_PAINT:
                return 0;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    
    GLPGWindowImpl_Win32::GLPGWindowImpl_Win32() {
        hInstance = GetModuleHandle(NULL);
        windowWidth = 640U;
        windowHeight = 480U;
    }

    GLPGWindowImpl_Win32::~GLPGWindowImpl_Win32() {
        //TODO
    }

    bool GLPGWindowImpl_Win32::CreateWindowImpl(uint32_t width, uint32_t height) {
        windowWidth = width;
        windowHeight = height;
        RECT windowRect = { 0, 0, static_cast<int32_t>(windowWidth), static_cast<int32_t>(windowHeight)};
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = "GLPG Window Class";
        wc.style = CS_OWNDC;

        if (!RegisterClass(&wc)) {
            std::cerr << "GLPG ERROR: Failed to register Window Class for Win32 Window" << std::endl;
            return false;
        }

        hWindow = CreateWindowA(wc.lpszClassName, "Test", WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left,
                      windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance,
                      nullptr);
        if (!hWindow) {
            std::cerr << "Failed to create native window\n";
            return false;
        } else {
            hDc = GetDC(hWindow);
            ShowWindow(hWindow, SW_SHOWNORMAL);
        }

        temphWindow = CreateWindowA(wc.lpszClassName, "Test", WS_OVERLAPPEDWINDOW,
                                    CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left,
                                    windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance,
                                    nullptr);
        if (!temphWindow) {
            std::cerr << "GLPG ERROR: Failed to create temp native window\n";
            return false;
        } else {
            temphDc = GetDC(temphWindow);
        }

        return true;
    }

    uint32_t GLPGWindowImpl_Win32::GetWidthImpl() {
        return windowWidth;
    }

    uint32_t GLPGWindowImpl_Win32::GetHeightImpl() {
        return windowHeight;
    }

    HWND& GLPGWindowImpl_Win32::GetWin32WindowHandle() {
        return hWindow;
    }

    HDC& GLPGWindowImpl_Win32::GetWin32HDC() {
        return hDc;
    }

    HDC& GLPGWindowImpl_Win32::GetTempWin32HDC() {
        return temphDc;
    }

    void* GLPGWindowImpl_Win32::GetWindowHandle() {
        return reinterpret_cast<void*>(&hWindow);
    }
} // namespace GLPG

