#include "GLPGContext.hpp"
#include "internal/GLPGContextImpl_GL.hpp"

#include <iostream>

// Export NvOptimusEnablement
// as 0x00000001 so that preference is given to
// Nvidia GPUs for GL rendering on Optimus systems.
extern "C" {
 _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

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
