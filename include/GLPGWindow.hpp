#pragma once

#include <iostream>

#ifdef __linux__
#include <xcb/xcb.h>
#elif defined _WIN32
#include <Windows.h>
#endif
namespace GLPG {

    class GLPGWindow
    {
        private:
            uint32_t windowWidth;
            uint32_t windowHeight;
#ifdef __linux__
            xcb_connection_t *xcbConnection;
            xcb_screen_t *xcbScreen;
            xcb_window_t xcbWindow;

            bool createNativeXcbWindow();
#elif defined _WIN32
            HWND hWindow;
            HINSTANCE hInstance;
            WNDCLASS wc = {};

            bool createNativeWin32Window();
#endif
        public:
            /* Constructor */
            GLPGWindow(uint32_t, uint32_t);

            /* Getters */
            inline uint32_t getWindowWidth() const;
            inline uint32_t getWindowHeight() const;

            /* Functions */
            bool createNativeWindow();
#ifdef __linux__
            xcb_window_t getNativeHandle() const;
            xcb_connection_t *getConnection() const;
#elif defined _WIN32
            HDC getNativeHandle() const;
#endif
    };

    inline uint32_t GLPGWindow::getWindowWidth() const
    {
        return windowWidth;
    }

    inline uint32_t GLPGWindow::getWindowHeight() const
    {
        return windowHeight;
    }
#ifdef __linux__
    inline xcb_connection_t *GLPGWindow::getConnection() const {
        return xcbConnection;
    }
#endif
}
