#include "internal/GLPGContextImpl_Platform_EGL.hpp"
#include "internal/GLPGContextImpl_GL.hpp"
#include "GLPGWindow.hpp"
#include "internal/GLPGWindowImpl_Xcb.hpp"
#include <cassert>

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLGETSTRINGIPROC glGetStringi;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding;
PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLPATHCOMMANDSNVPROC glPathCommandsNV;
PFNGLPATHSTRINGNVPROC glPathStringNV;
PFNGLPATHPARAMETERINVPROC glPathParameteriNV;
PFNGLPATHPARAMETERFNVPROC glPathParameterfNV;
PFNGLSTENCILFILLPATHNVPROC glStencilFillPathNV;
PFNGLSTENCILSTROKEPATHNVPROC glStencilStrokePathNV;
PFNGLCOVERFILLPATHNVPROC glCoverFillPathNV;
PFNGLCOVERSTROKEPATHNVPROC glCoverStrokePathNV;
PFNGLGENFRAMEBUFFERSPROC glGenFrameBuffers;
PFNGLBINDFRAMEBUFFERPROC glBindFrameBuffer;
PFNGLGENRENDERBUFFERSPROC glGenRenderBuffers;
PFNGLBINDRENDERBUFFERPROC glBindRenderBuffer;
PFNGLRENDERBUFFERSTORAGEPROC glRenderBufferStorage;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFrameBuffeRenderBuffer;
PFNGLFRAMEBUFFERTEXTUREPROC glFrameBufferTexture;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFrameBufferStatus;

namespace GLPG {
    void *GLPGContextImpl_Platform_EGL::LoadGLFunction(const char *name) {
        assert(name != nullptr);
        void *f = (void *)eglGetProcAddress(name);
        if (!f) {
            std::cerr << "GLPG ERROR: Failed to load GL Function: " << name << std::endl;
            return nullptr;
        }
        return f;
    }

    void GLPGContextImpl_Platform_EGL::LoadGLFunctionPointers() {
        glGenBuffers = (PFNGLGENBUFFERSPROC)LoadGLFunction("glGenBuffers");
        glBindBuffer = (PFNGLBINDBUFFERPROC)LoadGLFunction("glBindBuffer");
        glBufferData = (PFNGLBUFFERDATAPROC)LoadGLFunction("glBufferData");
        glGetStringi = (PFNGLGETSTRINGIPROC)LoadGLFunction("glGetStringi");
        glCreateShader = (PFNGLCREATESHADERPROC)LoadGLFunction("glCreateShader");
        glDeleteShader = (PFNGLDELETESHADERPROC)LoadGLFunction("glDeleteShader");
        glShaderSource = (PFNGLSHADERSOURCEPROC)LoadGLFunction("glShaderSource");
        glCompileShader = (PFNGLCOMPILESHADERPROC)LoadGLFunction("glCompileShader");
        glGetShaderiv = (PFNGLGETSHADERIVPROC)LoadGLFunction("glGetShaderiv");
        glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)LoadGLFunction("glGetShaderInfoLog");
        glCreateProgram = (PFNGLCREATEPROGRAMPROC)LoadGLFunction("glCreateProgram");
        glAttachShader = (PFNGLATTACHSHADERPROC)LoadGLFunction("glAttachShader");
        glLinkProgram = (PFNGLLINKPROGRAMPROC)LoadGLFunction("glLinkProgram");
        glGetProgramiv = (PFNGLGETPROGRAMIVPROC)LoadGLFunction("glGetProgramiv");
        glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)LoadGLFunction("glGetProgramInfoLog");
        glUseProgram = (PFNGLUSEPROGRAMPROC)LoadGLFunction("glUseProgram");
        glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)LoadGLFunction("glGenVertexArrays");
        glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)LoadGLFunction("glBindVertexArray");
        glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)LoadGLFunction("glVertexAttribPointer");
        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)LoadGLFunction("glEnableVertexAttribArray");
        glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC)LoadGLFunction("glVertexArrayAttribBinding");
        glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC)LoadGLFunction("glVertexArrayVertexBuffer");
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)LoadGLFunction("glGetUniformLocation");
        glUniform4f = (PFNGLUNIFORM4FPROC)LoadGLFunction("glUniform4f");
        glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)LoadGLFunction("glUniformMatrix4fv");
        glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)LoadGLFunction("glGenerateMipmap");
        glPathCommandsNV = (PFNGLPATHCOMMANDSNVPROC)LoadGLFunction("glPathCommandsNV");
        glPathStringNV = (PFNGLPATHSTRINGNVPROC)LoadGLFunction("glPathStringNV");
        glPathParameteriNV = (PFNGLPATHPARAMETERINVPROC)LoadGLFunction("glPathParameteriNV");
        glPathParameterfNV = (PFNGLPATHPARAMETERFNVPROC)LoadGLFunction("glPathParameterfNV");
        glStencilFillPathNV = (PFNGLSTENCILFILLPATHNVPROC)LoadGLFunction("glStencilFillPathNV");
        glStencilStrokePathNV = (PFNGLSTENCILSTROKEPATHNVPROC)LoadGLFunction("glStencilStrokePathNV");
        glCoverFillPathNV = (PFNGLCOVERFILLPATHNVPROC)LoadGLFunction("glCoverFillPathNV");
        glGenFrameBuffers = (PFNGLGENFRAMEBUFFERSPROC)LoadGLFunction("glGenFrameBuffers");
        glBindFrameBuffer = (PFNGLBINDFRAMEBUFFERPROC)LoadGLFunction("glBindFrameBuffer");
        glGenRenderBuffers = (PFNGLGENRENDERBUFFERSPROC)LoadGLFunction("glGenRenderBuffers");
        glBindRenderBuffer = (PFNGLBINDRENDERBUFFERPROC)LoadGLFunction("glBindRenderBuffer");
        glRenderBufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)LoadGLFunction("glRenderBufferStorage");
        glFrameBuffeRenderBuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)LoadGLFunction("glFrameBuffeRenderBuffer");
        glFrameBufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)LoadGLFunction("glFrameBufferTexture");
        glCheckFrameBufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)LoadGLFunction("glCheckFrameBufferStatus");

    }

    bool GLPGContextImpl_Platform_EGL::InitializeGLPlatformContext(uint32_t majorVersion = 4,
                                                                   uint32_t minorVersion = 5) {
        EGLBoolean ret;
        EGLint numConfig;
        EGLint const config_attrib_list[] = {
            EGL_RED_SIZE, 1,
            EGL_GREEN_SIZE, 1,
            EGL_BLUE_SIZE, 1,
            EGL_NONE
        };
        this->majorVersion = majorVersion;
        this->minorVersion = minorVersion;
        GLPGWindowImpl_Xcb* windowImpl = dynamic_cast<GLPGWindowImpl_Xcb*>(GLPG::GLPGWindow::GetImplInstance());
        if (!windowImpl) {
            std::cerr << "GLPG Error: Internal Failure" << std::endl;
            return false;
        }

        EGLint const context_attrib_list[] = {
            EGL_CONTEXT_MAJOR_VERSION, static_cast<EGLint>(this->majorVersion),
            EGL_CONTEXT_MINOR_VERSION, static_cast<EGLint>(this->minorVersion),
            EGL_NONE
        };

        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            std::cerr << "GLPG ERROR: Failed to create EGL Display" << std::endl;
            return false;
        }

        ret = eglInitialize(display, NULL, NULL);
        if (!ret) {
            std::cerr << "GLPG ERROR: Failed to initialize EGL Display" << std::endl;
            return false;
        }

        ret = eglChooseConfig(display, config_attrib_list, &config, 1, &numConfig);
        if (!ret) {
            std::cerr << "GLPG ERROR: Failed to choose EGLConfig" << std::endl;
            return false;
        }

        if ((eglQueryAPI() != EGL_OPENGL_API) && !(eglBindAPI(EGL_OPENGL_API))) {
            std::cerr << "GLPG ERROR: Failed to bind OpenGL to EGL\n";
            return false;
        }

        context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attrib_list);
        if (context == EGL_NO_CONTEXT) {
            std::cerr << "GLPG ERROR: Failed to create EGLContext" << std::endl;
            std::cout << "EGL Error: " << std::hex << eglGetError() << std::endl;
            return false;
        }

        surface = eglCreateWindowSurface(display, config, windowImpl->GetXcbWindowHandle(), NULL);
        if (surface == EGL_NO_SURFACE) {
            std::cerr << "GLPG ERROR: Failed to create EGLSurface" << std::endl;
            return false;
        }

        ret = eglMakeCurrent(display, surface, surface, context);
        if (!ret) {
            std::cerr << "GLPG ERROR: Failed to make EGLContext current" << std::endl;
            return false;
        }

        LoadGLFunctionPointers();
        std::cerr << "GLPG INFO: GL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cerr << "GLPG INFO: GPU: " << glGetString(GL_RENDERER) << std::endl;

        return true;
    }

    bool GLPGContextImpl_Platform_EGL::PlatformSwapBuffers() {
        eglSwapBuffers(display, surface);
        return true;
    }
} // namespace GLPG



