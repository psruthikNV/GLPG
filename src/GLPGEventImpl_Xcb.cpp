#include "GLPGEvent.hpp"
#include "GLPGWindow.hpp"
#include "internal/GLPGEventImpl_Xcb.hpp"

namespace GLPG {
    GLPGEventImpl_Xcb::GLPGEventImpl_Xcb() {
        connection = nullptr;
        xkbCtx = nullptr;
        keymap = nullptr;
        state = nullptr;
    }
    GLPGEventImpl_Xcb::~GLPGEventImpl_Xcb() {
        //TODO
    }

    bool GLPGEventImpl_Xcb::CreateEventLoopImpl() {
        GLPGWindowImpl_Xcb* windowImpl = dynamic_cast<GLPGWindowImpl_Xcb*>(GLPG::GLPGWindow::GetImplInstance());
        connection = windowImpl->GetXcbConnectionHandle();
        if (!connection) {
            std::cout << "GLPG ERROR: Internal error\n";
            return false;
        }
        if (!xkb_x11_setup_xkb_extension(connection, XKB_X11_MIN_MAJOR_XKB_VERSION,
                                         XKB_X11_MIN_MINOR_XKB_VERSION,
                                         XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS,
                                         NULL, NULL, &first_xkb_event, NULL)) {
            std::cerr << "Failed to setup XKB extensions\n";
            return false;
        }

        if (!(xkbCtx = xkb_context_new(XKB_CONTEXT_NO_FLAGS))) {
            std::cerr << "Failed to create new Xkb context\n";
            return false;
        }

        kb_device_id = xkb_x11_get_core_keyboard_device_id(connection);
        if (kb_device_id == -1) {
            std::cerr << "Failed to find keyboard device\n";
            return false;
        }

        struct xkb_keymap *new_keymap;
        new_keymap = xkb_x11_keymap_new_from_device(xkbCtx, connection,
                                                    kb_device_id,
                                                    XKB_KEYMAP_COMPILE_NO_FLAGS);
        if (!new_keymap) {
            std::cerr << "Failed to update keymap\n";
            return false;
        }

        struct xkb_state *new_state = xkb_x11_state_new_from_device(new_keymap, connection,
                                                                    kb_device_id);
        if (!new_state) {
            std::cerr << "Failed to update state\n";
            return false;
        }

        if (keymap) {
            std::cerr << "Keymap updated\n";
        }

        if (state) {
            //xkb_state_unref(state);
        }
        if (keymap) {
            xkb_keymap_unref(keymap);
        }
        keymap = new_keymap;
        state = new_state;
        return true;
    }

    GLPGEvent GLPGEventImpl_Xcb::GetEventImpl() {
        GLPGEvent rv = GLPGEvent::WindowCreate;
        if (xcb_generic_event_t *event;
            event = xcb_poll_for_event (connection)) {
            switch (event->response_type & ~0x80)
            {
                case XCB_BUTTON_PRESS:
                    {
                        xcb_button_press_event_t *press = (xcb_button_press_event_t *)event;
                        break;
                    }
                case XCB_BUTTON_RELEASE:
                    {
                        xcb_button_release_event_t *release = (xcb_button_release_event_t *)event;
                        break;
                    }
                case XCB_KEY_PRESS:
                    {
                        xcb_key_press_event_t *press = (xcb_key_press_event_t *)event;
                        xkb_keycode_t keycode = press->detail;
                        switch (xkb_state_key_get_one_sym(state, press->detail))
                        {
                            case XKB_KEY_Escape:
                                rv = GLPGEvent::Key_Escape;
                                break;
                            case XKB_KEY_BackSpace:
                                rv = GLPGEvent::Key_Backspace;
                                break;
                            case XKB_KEY_Tab:
                                rv = GLPGEvent::Key_Tab;
                                break;
                            case XKB_KEY_Linefeed:
                                rv = GLPGEvent::Key_Linefeed;
                                break;
                            case XKB_KEY_Scroll_Lock:
                                rv = GLPGEvent::Key_Scroll_lock;
                                break;
                            case XKB_KEY_Pause:
                                rv = GLPGEvent::Key_Pause;
                                break;
                            case XKB_KEY_Return:
                                rv = GLPGEvent::Key_Return;
                                break;
                            case XKB_KEY_Clear:
                                rv = GLPGEvent::Key_Clear;
                                break;
                            case XKB_KEY_Sys_Req:
                                rv = GLPGEvent::Key_SysReq;
                                break;
                            case XKB_KEY_Delete:
                                rv = GLPGEvent::Key_Delete;
                                break;
                            case XKB_KEY_W:
                            case XKB_KEY_w:
                                rv = GLPGEvent::Key_W;
                                std::cout << "W\n";
                                break;
                            case XKB_KEY_A:
                            case XKB_KEY_a:
                                rv = GLPGEvent::Key_A;
                                break;
                            case XKB_KEY_S:
                            case XKB_KEY_s:
                                rv = GLPGEvent::Key_S;
                                break;
                            case XKB_KEY_D:
                            case XKB_KEY_d:
                                rv = GLPGEvent::Key_D;
                                break;
                        }
                    }
            }
        }
        return rv;
    }

} // namespace GLPG
