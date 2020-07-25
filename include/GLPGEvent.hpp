#pragma once

#include <stdint.h>
#include "GLPGEvent_Enums.hpp"
#include "internal/GLPGEventImpl.hpp"

namespace GLPG {
    class GLPGEventLoop {
        public:
            GLPGEvent GetEvent(void);
            bool InitializeEventLoop();
            GLPGEventLoop();
            ~GLPGEventLoop();
        private:
            GLPGEventImpl *_eventImpl;
    };

} // namespace GLPG

