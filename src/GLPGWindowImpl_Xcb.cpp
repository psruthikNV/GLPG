#include "internal/GLPGWindowImpl_Xcb.hpp"

namespace GLPG {
    GLPGWindowImpl_Xcb::GLPGWindowImpl_Xcb() {
        xcbConnection = nullptr;
        xcbScreen = nullptr;
        //TODO: Use the current display resolution as the Window width and height
        windowWidth = 640;
        windowHeight = 480;
    }

    GLPGWindowImpl_Xcb::~GLPGWindowImpl_Xcb() {
        //TODO
    }

    bool GLPGWindowImpl_Xcb::CreateWindowImpl(uint32_t width, uint32_t height) {
        windowWidth = width;
        windowHeight = height;
        xcbConnection = xcb_connect(NULL, NULL);
        if (xcb_connection_has_error(xcbConnection)) {
            std::cerr << "GLPG ERROR: Failed to create XCB Connection" << std::endl;
            return false;
        }

        uint32_t valueMask = XCB_CW_EVENT_MASK;
        uint32_t valueList[1] = { XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                                  XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                                  XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_BUTTON_1_MOTION |
                                  XCB_EVENT_MASK_BUTTON_2_MOTION | XCB_EVENT_MASK_BUTTON_3_MOTION |
                                  XCB_EVENT_MASK_BUTTON_4_MOTION | XCB_EVENT_MASK_BUTTON_5_MOTION |
                                  XCB_EVENT_MASK_BUTTON_MOTION };
        xcbScreen = xcb_setup_roots_iterator(xcb_get_setup(xcbConnection)).data;
        xcbWindow = xcb_generate_id(xcbConnection);

        xcb_create_window(xcbConnection, XCB_COPY_FROM_PARENT, xcbWindow,
                          xcbScreen->root, 0, 0, windowWidth, windowHeight,
                          10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                          xcbScreen->root_visual, valueMask, valueList);
        xcb_map_window(xcbConnection, xcbWindow);
        xcb_flush(xcbConnection);
        return true;
    }

    uint32_t GLPGWindowImpl_Xcb::GetWidthImpl() {
        return windowWidth;
    }

    uint32_t GLPGWindowImpl_Xcb::GetHeightImpl() {
        return windowHeight;
    }

    xcb_window_t& GLPGWindowImpl_Xcb::GetXcbWindowHandle() {
        return xcbWindow;
    }

    xcb_connection_t* GLPGWindowImpl_Xcb::GetXcbConnectionHandle() {
        return xcbConnection;
    }

} // namespace GLPG

