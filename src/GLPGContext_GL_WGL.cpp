#include "GLPGContext.hpp"
#include "internal/GLPGContextImpl_GL.hpp"
#include "internal/GLPGContextImpl_EGL.hpp"

namespace GLPG {
    bool GLPGContext::InitializeContext(GLPGWindow &window) {
        if (!contextImpl) {
            contextImpl = new GLPGContextImpl_GL;
        }
    }
} // namespace GLPG
