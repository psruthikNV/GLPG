#pragma once

namespace GLPG {
    class GLPGContextImpl {
        public:
            virtual bool InitializeContextImpl() = 0;
            virtual bool SwapBuffers() = 0;
            virtual ~GLPGContextImpl() {};
    };
} // namespace GLPG
