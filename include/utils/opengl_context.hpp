#include "native_window.hpp"
#ifdef __linux__
#include <EGL/egl.h>
#endif
#include <GL/gl.h>

class glContext
{
    private:
        struct glContextConfig
        {
            uint32_t glContextMinVersion;
            uint32_t glContextMajorVersion;
        };
        glContextConfig glConfig;
        #ifdef __linux__
        struct eglResources
        {
            EGLDisplay display;
            EGLConfig config;
            EGLContext context;
            EGLSurface surface;
        };
        eglResources eglRes;
        bool initializeEglBackend(nativeWindow &window);
        #elif defined _WIN32
        struct wglResources
        {
            HGLRC tempHglrc;
            HGLRC hGlrc;
            HDC hDc;
            int pixelFormat;
        };
        wglResources wglRes;
        bool initializeWglBackend(nativeWindow &window);
        bool createDummyGlContext();
        bool createActualGlContext();
        #endif
        bool loadGlFunctions();
    public:
        glContext();
        glContext(nativeWindow &window);
        glContext(nativeWindow &window, uint32_t majorVersion, uint32_t minorVersion);
        bool initializeGlContext(nativeWindow &window, uint32_t majorVersion,
                                 uint32_t minorVersion);
        bool swapBuffers();
};
