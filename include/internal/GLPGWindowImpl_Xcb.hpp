#pragma once

#include <xcb/xcb.h>
#include "internal/GLPGWindowImpl.hpp"

namespace GLPG {
    class GLPGWindowImpl_Xcb : public GLPGWindowImpl {
        public:
            GLPGWindowImpl_Xcb();
            ~GLPGWindowImpl_Xcb();
            bool CreateWindowImpl(uint32_t width,
                                  uint32_t height) override final;
            uint32_t GetWidthImpl() override final;
            uint32_t GetHeightImpl() override final;
            xcb_window_t& GetXcbWindowHandle();
            xcb_connection_t* GetXcbConnectionHandle();
            void* GetWindowHandle() ;
        private:
            xcb_connection_t *xcbConnection;
            xcb_screen_t *xcbScreen;
            xcb_window_t xcbWindow;
            uint32_t windowWidth;
            uint32_t windowHeight;
    };      
} // namespace GLPG


