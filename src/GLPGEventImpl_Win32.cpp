#include "GLPGEvent.hpp"
#include "GLPGWindow.hpp"
#include "internal/GLPGEventImpl_Win32.hpp"
#include "internal/GLPGWindowImpl_Win32.hpp"

namespace GLPG {

    static void FullScreenTransition()
    {
        DEVMODE desktopMode = {};
        auto ret = EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &desktopMode);
        if (!ret) {
            std::cerr << "Failed to get display mode\n";
            return;
        }
        std::cout << "Width: " << desktopMode.dmPelsWidth << "\n";

        GLPGWindowImpl_Win32* windowImpl = dynamic_cast<GLPGWindowImpl_Win32*>(GLPG::GLPGWindow::GetImplInstance());
        if (!windowImpl) {
            std::cerr << "GLPG Error: Internal Failure" << std::endl;
            return;
        }
        SetWindowLongPtr(windowImpl->GetWin32WindowHandle(), GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
        SetWindowLongPtr(windowImpl->GetWin32WindowHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowPos(windowImpl->GetWin32WindowHandle(), HWND_TOPMOST, 0, 0, desktopMode.dmPelsWidth, desktopMode.dmPelsHeight, SWP_SHOWWINDOW);
        auto ret_2 = ChangeDisplaySettings(&desktopMode, CDS_FULLSCREEN);
        if (ret_2 != DISP_CHANGE_SUCCESSFUL) {
            std::cerr << "Failed to set FS\n";
        }
        ShowWindow(windowImpl->GetWin32WindowHandle(), SW_MAXIMIZE);
    }

    GLPGEventImpl_Win32::GLPGEventImpl_Win32() {
    }

    GLPGEventImpl_Win32::~GLPGEventImpl_Win32() {

    }

    bool GLPGEventImpl_Win32::CreateEventLoopImpl() {
        GLPGWindowImpl_Win32* windowImpl = dynamic_cast<GLPGWindowImpl_Win32*>(GLPG::GLPGWindow::GetImplInstance());
        return true;
    }

    GLPGEvent GLPGEventImpl_Win32::GetEventImpl() {
        GLPGEvent rv = GLPGEvent::WindowCreate;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            switch (msg.message)
            {
                case WM_KEYDOWN:
                    switch (msg.wParam)
                    {
                        case VK_LEFT:
                            rv = GLPGEvent::LeftArrow;
                            break;
                        case VK_RIGHT:
                            rv = GLPGEvent::RightArrow;
                            break;
                        case VK_UP:
                            rv = GLPGEvent::UpArrow;
                            break;
                        case VK_DOWN:
                            rv = GLPGEvent::DownArrow;
                            break;
                        case VK_ESCAPE:
                            rv = GLPGEvent::Key_Escape;
                            break;
                        case VK_RETURN:
                            rv = GLPGEvent::Key_Return;
                            break;
                        case 0x57:
                            rv = GLPGEvent::Key_W;
                            break;
                        case 0x53:
                            rv = GLPGEvent::Key_S;
                            break;
                        case 0x41:
                            rv = GLPGEvent::Key_A;
                            break;
                        case 0x44:
                            rv = GLPGEvent::Key_D;
                            break;
                        case VK_LMENU:
                            rv = GLPGEvent::Key_Alt;
                            break;
                        case 0x30:
                            rv = GLPGEvent::Key_0;
                            break;
                        case 0x31:
                            rv = GLPGEvent::Key_1;
                            break;
                        case 0x32:
                            rv = GLPGEvent::Key_2;
                            break;
                        case 0x33:
                            rv = GLPGEvent::Key_3;
                            break;
                        case 0x34:
                            rv = GLPGEvent::Key_4;
                            break;
                    }
                    break;
                case WM_SYSKEYDOWN:
                    switch (msg.wParam)
                    {
                        case VK_RETURN:
                            rv = GLPGEvent::Key_Alt_Return;
                            FullScreenTransition();
                            break;
                    }
                    break;
                case WM_MOUSEWHEEL:
                    if (GET_WHEEL_DELTA_WPARAM(msg.wParam) > 0) {
                        rv = GLPGEvent::MouseWheel_Up;
                    } else {
                        rv = GLPGEvent::MouseWheel_Down;
                    }
                    break;
                case WM_LBUTTONDOWN:
                    uint16_t xPos = msg.lParam & 0xFFFF;
                    uint16_t yPos = (msg.lParam >> 16U) & 0xFFFF;
                    std::cout << "xPos: " << xPos << " yPos: " << yPos << "\n";
                    break;
            }
        }
        return rv;
    }

} // namespace GLPG
