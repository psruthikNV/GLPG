#include "GLPGWindow.hpp"

using namespace GLPG;
GLPGWindow::GLPGWindow(uint32_t w, uint32_t h)
{
    windowWidth = w;
    windowHeight = h;
}

bool GLPGWindow::createNativeWindow()
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

#ifdef __linux__
bool GLPGWindow::createNativeXcbWindow()
{
    xcbConnection = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(xcbConnection)) {
        std::cout << "Failed to create XCB Connection" << std::endl;
        return false;
    } else {
        uint32_t valueMask = XCB_CW_EVENT_MASK;
        // No documentation exists for xcb_event_mask_t on the web so
        // including whatever I think is necessary
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
}

xcb_window_t GLPGWindow::getNativeHandle() const
{
    return xcbWindow;
}
#elif defined _WIN32

HDC GLPGWindow::getNativeHandle() const
{
    return (GetDC(hWindow));
}

static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
	{
	case WM_CREATE:
        std::cout << "Creating Native Window\n";
	break;
	case WM_CLOSE:
		PostQuitMessage(0);
    break;
    default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool GLPGWindow::createNativeWin32Window()
{
    hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "Window Class";
    wc.style = CS_OWNDC;

    if (!RegisterClass(&wc)) {
        std::cout << "Failed to register Window Class for Win32 Window" << std::endl;
        return false;
    }

    hWindow = CreateWindow("Window Class", "Test", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                           0, 0, 800, 600, 0, 0, hInstance, 0);
    return true;
}

#endif
