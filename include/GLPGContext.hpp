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

namespace glpg {

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
            bool loadWglFunctionPointers();
#endif
            void *loadGLFunction(const char *name);
            void initializeGLFunctionPointers();

        public:
            glContext();
            glContext(nativeWindow &window);
            glContext(nativeWindow &window, uint32_t majorVersion, uint32_t minorVersion);
            bool initializeGlContext(nativeWindow &window, uint32_t majorVersion,
                    uint32_t minorVersion);
            bool swapBuffers();
    };
}
