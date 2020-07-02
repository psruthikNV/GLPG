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
                        std::cout << "Button pressed, detail: " << press->detail << "\n";
                        break;
                    }
                case XCB_BUTTON_RELEASE:
                    {
                        xcb_button_release_event_t *release = (xcb_button_release_event_t *)event;
                        std::cout << "Button released, detail: " << release->detail << "\n";
                        break;
                    }
                case XCB_KEY_PRESS:
                    {
                        xcb_key_press_event_t *press = (xcb_key_press_event_t *)event;
                        std::cout <<"Key pressed, detail: " << press->detail << "\n";
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
