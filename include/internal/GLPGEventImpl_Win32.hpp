#pragma once

#include "GLPGEvent.hpp"
#include "internal/GLPGWindowImpl_Win32.hpp"

namespace GLPG {
    class GLPGEventImpl_Win32 : public GLPGEventImpl {
        public:
            GLPGEventImpl_Win32();
            ~GLPGEventImpl_Win32();
            GLPGEvent GetEventImpl() override final;
            bool CreateEventLoopImpl() override final;
        private:
            MSG msg = {};
    };
}
