#pragma once

#include "internal/GLPGWindowImpl.hpp"
#undef CreateWindow

namespace GLPG {
    class GLPGWindow {
        public:
            static GLPGWindow* GetInstance();
            static GLPGWindowImpl* GetImplInstance();
            bool CreateWindow(uint32_t width, uint32_t height) const;
            uint32_t GetWindowWidth() const;
            uint32_t GetWindowHeight() const;
            ~GLPGWindow();
        protected:
            GLPGWindow();
        private:
            static GLPGWindowImpl* _windowImpl;
            static GLPGWindow* _instance;
    };

} // namespace GLPG

