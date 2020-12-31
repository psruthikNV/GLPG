#include "GLPGEvent.hpp"
#include "GLPGWindow.hpp"
#include "internal/GLPGEventImpl_Win32.hpp"

namespace GLPG {

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
                        case 0x57:
                            rv = GLPGEvent::Key_W;
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
            }
        }
        return rv;
    }

} // namespace GLPG