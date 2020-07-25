#pragma once

#include "internal/GLPGContextImpl.hpp"

namespace GLPG {
    class GLPGContext {
        public:
            GLPGContext();
            ~GLPGContext();
            bool InitializeContext();
            bool SwapBuffers();
        private:
            GLPGContextImpl *contextImpl;
    };
} // namespace GLPG

