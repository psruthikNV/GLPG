#pragma once

#include <cstdint>

// Platform specific code must
// be implemented as a concrete
// sub class of this abstract class
namespace GLPG {
    class GLPGContextImpl_Platform {
        public:
            virtual bool InitializeGLPlatformContext(uint32_t min = 4, uint32_t maj = 5) = 0;
            virtual bool PlatformSwapBuffers() = 0;
            virtual ~GLPGContextImpl_Platform() {} ;
    };
} // namespace GLPG
