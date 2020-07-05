#pragma once

// xkb.h uses the identifier explicit
// which is a keyword in c++. WAR it
// with this macro
#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit
#include <xkbcommon/xkbcommon-x11.h>

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
        Key_D,
        Key_Escape,
        Key_Scroll_lock,
        Key_Pause,
        Key_Return,
        Key_Backspace,
        Key_Tab,
        Key_Linefeed,
        Key_Clear,
        Key_SysReq,
        Key_Delete,
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
        struct xkb_context *xkbCtx;
        struct xkb_keymap *keymap;
        struct xkb_state *state;
        int32_t kb_device_id;
        uint8_t first_xkb_event;
#endif

        public:
        GLPGEventLoop(void);
        GLPGEventLoop(GLPGWindow *window);
        GLPGEvent GetEvent(void);
        bool SetActiveCamera(GLPGCamera *camera);
        GLPGCamera& GetActiveCamera();
    };
}
    
