#pragma once

#include "GLPGWindow.hpp"

namespace GLPG {
    enum class GLPGEvent : uint32_t {
        WindowClose,
        WindowCreate,
    };

    class GLPGEventLoop {
        bool loopInited;
        bool loopTerminate;
        bool windowRegistered;
#ifdef _WIN32
        HWND hWindow;
        MSG message;
#endif

        public:
        GLPGEventLoop(void);
        GLPGEvent GetEvent(void);
        //bool RegisterGLPGWindow(nativeWindow &window);
    };
}
    
