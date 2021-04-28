#include "internal/GLPGContextImpl_GL.hpp"
#include "internal/GLPGContextImpl_Platform_EGL.hpp"

namespace GLPG {

#ifdef GLPG_IS_DEBUG
    void DebugMessageCallback(GLenum source,
                              GLenum type,
                              GLuint id,
                              GLenum severity,
                              GLsizei length,
                              const GLchar* message,
                              const void* userParam ) {
        std::cerr << "GLPG GL Debug: "  << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") \
                  << " severity = " << severity << " message = " << message << "\n";
    }
#endif

    bool GLPGContextImpl_GL::InitializeContextImpl() {
        if (platformImpl == nullptr) {
            platformImpl = new GLPGContextImpl_Platform_EGL;
        }
        bool ret = platformImpl->InitializeGLPlatformContext();
#ifdef GLPG_IS_DEBUG
        if (ret) {
            ret = glDebugMessageCallback(DebugMessageCallback, nullptr);
        }
#endif
        return ret;
    }

} // namespace GLPG
