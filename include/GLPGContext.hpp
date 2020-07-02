#pragma once

#include "GLPGWindow.hpp"
#ifdef __linux__
#include <EGL/egl.h>
#endif
#ifdef _WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#ifdef _WIN32
#include <gl\wglext.h>
#endif


#ifdef _WIN32
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
extern PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;
#endif
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLGETSTRINGIPROC glGetStringi;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding;
extern PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
extern PFNGLGENQUERIESARBPROC glGenQueries;
extern PFNGLBEGINQUERYARBPROC glBeginQuery;
extern PFNGLENDQUERYARBPROC glEndQuery;
extern PFNGLGETQUERYOBJECTIVARBPROC glGetQueryObjectiv;
extern PFNGLGETQUERYOBJECTI64VEXTPROC glGetQueryObjecti64v;
extern PFNGLGETQUERYOBJECTUI64VEXTPROC glGetQueryObjectui64v;

namespace GLPG {

    class GLPGContext
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
            bool initializeEglBackend(GLPGWindow &window);
#elif defined _WIN32
            struct wglResources
            {
                HGLRC tempHglrc;
                HGLRC hGlrc;
                HDC hDc;
                int pixelFormat;
            };
            wglResources wglRes;
            bool initializeWglBackend(GLPGWindow &window);
            bool createDummyGlContext();
            bool createActualGlContext();
            bool loadWglFunctionPointers();
#endif
            void *loadGLFunction(const char *name);
            void initializeGLFunctionPointers();

        public:
            GLPGContext();
            GLPGContext(GLPGWindow &window);
            GLPGContext(GLPGWindow &window, uint32_t majorVersion, uint32_t minorVersion);
            bool initializeGlContext(GLPGWindow &window, uint32_t majorVersion,
                    uint32_t minorVersion);
            bool swapBuffers();
            int getSwapInterval();
            bool setSwapInterval(int interval);
    };
}
