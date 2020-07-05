#include "GLPGEvent.hpp"

namespace GLPG {

    //TODO: Can this ctor be removed?
    GLPGEventLoop::GLPGEventLoop() {
        loopInited = true;
        loopTerminate = false;
        activeCamera = nullptr;
    }

    GLPGEventLoop::GLPGEventLoop(GLPGWindow *window) {
#ifdef __linux__
        connection = window->getConnection();
        if (!xkb_x11_setup_xkb_extension(connection, XKB_X11_MIN_MAJOR_XKB_VERSION,
                                         XKB_X11_MIN_MINOR_XKB_VERSION,
                                         XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS,
                                         NULL, NULL, &first_xkb_event, NULL)) {
            std::cerr << "Failed to setup XKB extensions\n";
        }

        if (!(xkbCtx = xkb_context_new(XKB_CONTEXT_NO_FLAGS))) {
            std::cerr << "Failed to create new Xkb context\n";
        }

        kb_device_id = xkb_x11_get_core_keyboard_device_id(connection);
        if (kb_device_id == -1) {
            std::cerr << "Failed to find keyboard device\n";
        } else {
            std::cerr << "Keyboard device id: " << kb_device_id << "\n";
        }
        struct xkb_keymap *new_keymap;
        new_keymap = xkb_x11_keymap_new_from_device(xkbCtx, connection,
                                                    kb_device_id,
                                                    XKB_KEYMAP_COMPILE_NO_FLAGS);
        if (!new_keymap) {
            std::cerr << "Failed to update keymap\n";
        }

        struct xkb_state *new_state = xkb_x11_state_new_from_device(new_keymap, connection,
                                                                    kb_device_id);
        if (!new_state) {
            std::cerr << "Failed to update state\n";
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
                
#endif
        loopInited = true;
        loopTerminate = false;
        activeCamera = nullptr;
    }

    GLPGEvent GLPGEventLoop::GetEvent() {
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
#elif defined __linux__
        if (xcb_generic_event_t *event;
            event = xcb_poll_for_event (connection)) {
            switch (event->response_type & ~0x80)
            {
                //TODO: Why the fuck is there no documentation on this?
                // I need to get xkbcommon for this shit? The fuck Linux?!
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
                                std::cout << "Return\n";
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
                        if (xkb_state_key_get_one_sym(state, press->detail) == XKB_KEY_Escape) { 
                            std::cout << "Escape\n";
                        }
                        break;
                    }
                default:
                    std::cout << "Unhandled event recieved\n";
            }
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
