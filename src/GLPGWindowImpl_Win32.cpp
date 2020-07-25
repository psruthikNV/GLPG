#include "internal/GLPGWindowImpl_Win32.hpp"

namespace GLPG {
    bool GLPGWindowImpl_Win32::CreateWindowImpl(uint32_t width, uint32_t height) {
        windowWidth = width;
        windowHeight = height;

        hInstance = GetModuleHandle(NULL);
        wc.lpfnWndProc = windowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = "GLPG Window Class";
        wc.style = CS_OWNDC;

        if (!RegisterClass(&wc)) {
            std::cerr << "GLPG ERROR: Failed to register Window Class for Win32 Window" << std::endl;
            return false;
        }

        hWindow = CreateWindow("GLPG Window", "Test", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               0, 0, windowWidth, windowHeight, hInstance, 0);
        return true;
    }

    uint32_t GLPGWindow::GetWidthImpl() {
        return windowWidth;
    }

    uint32_t GLPGWindow::GetHeightImpl() {
        return windowHeight;
    }

} // namespace GLPG

