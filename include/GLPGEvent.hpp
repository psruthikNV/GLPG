#pragma once

#include "GLPGWindow.hpp"
#include "GLPGCamera.hpp"

namespace GLPG {
    enum class GLPGEvent : uint32_t {
        WindowClose,
        WindowCreate,
        LeftArrow,
        DownArrow,
        UpArrow,
        RightArrow,
        Key_W,
        Key_S,
        Key_A,
        Key_D
    };

    class GLPGEventLoop {
        bool loopInited;
        bool loopTerminate;
        bool windowRegistered;
        GLPGCamera *activeCamera;
#ifdef _WIN32
        HWND hWindow;
        MSG message;
#elif defined __linux__
        xcb_connection_t *connection;
#endif

        public:
        GLPGEventLoop(void);
        GLPGEventLoop(GLPGWindow *window);
        GLPGEvent GetEvent(void);
        bool SetActiveCamera(GLPGCamera *camera);
        GLPGCamera& GetActiveCamera();
    };
}
    
