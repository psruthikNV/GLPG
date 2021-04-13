#pragma once

#include "internal/GLPGContextImpl_Platform.hpp"
#include "internal/GLPGContextImpl_GL.hpp"
#include <gl/wglext.h>

namespace GLPG {
    class GLPGContextImpl_Platform_WGL : public GLPGContextImpl_Platform {
        public:
            bool InitializeGLPlatformContext(uint32_t, uint32_t) override final;
            bool PlatformSwapBuffers() override final;
            //GLPGContextImpl_Platform_WGL();
        private:
            HGLRC tempHglrc;
            HGLRC hGlrc;
            HDC temphDc;
            HDC hDc;
            int pixelFormat;

            void *LoadGLFunction(const char *name);
            void LoadGLFunctionPointers();
            bool CreateDummyGLContext();
            bool CreateActualGLContext();
            bool LoadWGLFunctionPointers();
    };
} // namespace GLPG
