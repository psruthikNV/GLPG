#include "internal/GLPGContextImpl_GL.hpp"
#include "internal/GLPGContextImpl_Platform_WGL.hpp"

namespace GLPG {
    bool GLPGContextImpl_GL::InitializeContextImpl() {
        if (platformImpl == nullptr) {
            platformImpl = new GLPGContextImpl_Platform_WGL;
        }
        return platformImpl->InitializeGLPlatformContext();
    }
} // namespace GLPG
