#pragma once

#include <Windows.h>

namespace GLPG {
    class GLPGWindowImpl_Win32 : public GLPGWindowImpl final {
        public:
            bool CreateWindowImpl() override final;
            uint32_t GetWidthImpl() override final;
            uint32_t GetHeightImpl() override final;
        private:
            HWND hWindow;
            HINSTANCE hInstance;
            WNDCLASS wc = {};
            uint32_t windowWidth;
            uint32_t windowHeight;
    };
} // namespace GLPG

