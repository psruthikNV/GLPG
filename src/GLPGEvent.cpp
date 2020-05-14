#include "GLPGEvent.hpp"

namespace GLPG {

    GLPGEventLoop::GLPGEventLoop()
    {
        loopInited = true;
        loopTerminate = false;
        activeCamera = nullptr;
    }
/*
    bool GLPGEventLoop::RegisterGLPGWindow(nativeWindow &window)
    {
#ifdef _WIN32
        hWindow = window.getNativeHandle();
#endif
        windowRegistered = true;
        return true;
    }
    */

    GLPGEvent GLPGEventLoop::GetEvent()
    {
        GLPGEvent rv = GLPGEvent::WindowCreate;
#ifdef _WIN32
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
            switch (message.message)
            {
                case WM_QUIT:
                    rv = GLPGEvent::WindowClose;
                    break;
                case WM_KEYDOWN:
                    switch (message.wParam)
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
                        case 0x57:
                            rv = GLPGEvent::Key_W;
                            break;
                        case 0x41:
                            rv = GLPGEvent::Key_A;
                            break;
                        case 0x44:
                            rv = GLPGEvent::Key_D;
                            break;
                        case 0x53:
                            rv = GLPGEvent::Key_S;
                            break;
                        default: // Do nothing
                            break;
                    }
                default: // Do nothing
                    break;
            }
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
#endif
        return rv;
    }

    bool GLPGEventLoop::SetActiveCamera(GLPGCamera *camera)
    {
        if (camera) {
            activeCamera = camera;
            return true;
        } else {
            return false;
        }
    }

    GLPGCamera& GLPGEventLoop::GetActiveCamera()
    {
        return *activeCamera;
    }
};
