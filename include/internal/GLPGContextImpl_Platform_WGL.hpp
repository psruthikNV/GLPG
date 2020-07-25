#pragma once

#include <gl\wglext.h>

#include "internal/GLPGContextImpl_Platform.hpp"

// Forward declarations of WGL APIs
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

namespace GLPG {
    class GLPGContextImpl_Platform_WGL : public GLPGContextImpl_Platform {
        public:
            bool InitializeGLPlatformContext() override final;
            bool PlatformSwapBuffers() override final;
        private:
            HGLRC tempHglrc;
            HGLRC hGlrc;
            HDC hDc;
            int pixelFormat;
    };
} // namespace GLPG
