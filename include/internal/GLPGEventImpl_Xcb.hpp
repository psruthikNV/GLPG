#pragma once

#define explicit dont_use_cxx_explicit
#include <xcb/xcb.h>
#undef explicit
#include <xkbcommon/xkbcommon-x11.h>

#include "GLPGEvent.hpp"
#include "internal/GLPGWindowImpl_Xcb.hpp"

namespace GLPG {
    class GLPGEventImpl_Xcb : public GLPGEventImpl {
        public:
            GLPGEventImpl_Xcb();
            ~GLPGEventImpl_Xcb();
            GLPGEvent GetEventImpl() override final;
            bool CreateEventLoopImpl() override final;
        private:
            xcb_connection_t *connection;
            struct xkb_context *xkbCtx;
            struct xkb_keymap *keymap;
            struct xkb_state *state;
            int32_t kb_device_id;
            uint8_t first_xkb_event;
    };
}
