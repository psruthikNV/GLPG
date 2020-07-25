#pragma once

#include <EGL/egl.h>
#include "internal/GLPGContextImpl_Platform.hpp"

namespace GLPG {
    class GLPGContextImpl_Platform_EGL : public GLPGContextImpl_Platform {
        public:
            bool InitializeGLPlatformContext(uint32_t, uint32_t) override final;
            bool PlatformSwapBuffers() override final;
            GLPGContextImpl_Platform_EGL() {}
        private:
            EGLDisplay display;
            EGLConfig config;
            EGLContext context;
            EGLSurface surface;
            uint32_t majorVersion;
            uint32_t minorVersion;

            void *LoadGLFunction(const char *name);
            void LoadGLFunctionPointers();
    };
}; // namespace GLPG
