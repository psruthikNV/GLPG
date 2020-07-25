#include "GLPGContext.hpp"
#include "internal/GLPGContextImpl_GL.hpp"

#include <iostream>

namespace GLPG {

    GLPGContext::GLPGContext() {
        contextImpl = nullptr;
    }

    GLPGContext::~GLPGContext() {
        delete contextImpl;
    }

    bool GLPGContext::InitializeContext() {
        if (!contextImpl) {
            contextImpl = new GLPGContextImpl_GL;
        }
        return contextImpl->InitializeContextImpl();
    }

    bool GLPGContext::SwapBuffers() {
        return contextImpl->SwapBuffers();
    }

} // namespace GLPG
