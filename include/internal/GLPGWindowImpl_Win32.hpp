#pragma once

#include <Windows.h>
#include "internal/GLPGWindowImpl.hpp"

namespace GLPG {
    class GLPGWindowImpl_Win32 : public GLPGWindowImpl {
        public:
            GLPGWindowImpl_Win32();
            ~GLPGWindowImpl_Win32();
            bool CreateWindowImpl(uint32_t width,
                                  uint32_t height) override final;
            uint32_t GetWidthImpl() override final;
            uint32_t GetHeightImpl() override final;
            HWND& GetWin32WindowHandle();
            HDC& GetWin32HDC();
        private:
            HWND hWindow;
            HINSTANCE hInstance;
            HDC hDc;
            WNDCLASS wc = {};
            MSG msg = {};
            uint32_t windowWidth;
            uint32_t windowHeight;
    };
} // namespace GLPG

