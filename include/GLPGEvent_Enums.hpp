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
        MouseWheel_Down,
        Key_Alt_Return,
        Key_Alt,
        Key_0,
        Key_1,
        Key_2,
        Key_3,
        Key_4
    };
}
