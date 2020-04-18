#include "GLPGEvent.hpp"

namespace GLPG {

    GLPGEventLoop::GLPGEventLoop()
    {
        loopInited = true;
        loopTerminate = false;
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
            if (message.message == WM_QUIT) {
                rv = GLPGEvent::WindowClose;
            }
            if (message.message == WM_KEYDOWN) {
                std::cout << "Key Pressed\n";
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }
#endif
        return rv;
    }

};

