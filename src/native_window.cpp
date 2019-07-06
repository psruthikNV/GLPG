#include "utils/native_window.hpp"

nativeWindow::nativeWindow(uint32_t w, uint32_t h)
{
    windowWidth = w;
    windowHeight = h;
}

bool nativeWindow::createNativeWindow()
{
    bool ret;
#ifdef __linux__
    ret = createNativeXcbWindow();
#elif defined _WIN32
    ret = createNativeWin32Window();
#endif
    if (!ret) {
        std::cout << "Failed to create Native Window" << std::endl;
        return false;
    }
    return true;
}

bool nativeWindow::createNativeXcbWindow()
{
    xcbConnection = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(xcbConnection)) {
        std::cout << "Failed to create XCB Connection" << std::endl;
        return false;
    }
    xcbScreen = xcb_setup_roots_iterator(xcb_get_setup(xcbConnection)).data;
    xcbWindow = xcb_generate_id(xcbConnection);

    xcb_create_window(xcbConnection, XCB_COPY_FROM_PARENT, xcbWindow,
                      xcbScreen->root, 0, 0, windowWidth, windowHeight,
                      10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      xcbScreen->root_visual, 0, NULL);
    xcb_map_window(xcbConnection, xcbWindow);
    xcb_flush(xcbConnection);

    return true;
}

#ifdef __linux__
xcb_window_t nativeWindow::getNativeHandle() const
{
    return xcbWindow;
}
#endif
