#pragma once

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
        MouseWheel_Up,
        MouseWheel_Down
    };
}
