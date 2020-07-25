#include "internal/GLPGContextImpl_Platform_WGL.hpp"

namespace GLPG {
    void *GLPGContextImpl_Platform_WGL::LoadGLFunction(const char *name) {
        void *f = (void *)wglGetProcAddress(name);
        if (!f || (f == (void *)0x1) || (f == (void *)0x3)
            || (f == (void *)-1)) {
            HMODULE module = LoadLibrary("opengl32.dll");
            f = (void *)GetProcAddress(module, name);
            if (!f) {
                std::cerr << "GLPG ERROR: Failed to load GL Function: " << name << std::endl;
                return nullptr;
            }
        }
        return f;
    }
