#include "GLPGWindow.hpp"
#include "internal/GLPGContextImpl_Platform_WGL.hpp"
#include "internal/GLPGWindowImpl_Win32.hpp"


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
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

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

    void GLPGContextImpl_Platform_WGL::LoadGLFunctionPointers() {
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
    }

    bool GLPGContextImpl_Platform_WGL::LoadWGLFunctionPointers() {
    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)LoadGLFunction("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)LoadGLFunction("wglCreateContextAttribsARB");

    if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB) {
        std::cout << "Failed to load required WGL Functions" << std::endl;
        return false;
    }
    return true;
    }

    bool GLPGContextImpl_Platform_WGL::CreateDummyGLContext() {
        PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,
            8,
            0,
            0,
            0,
            0,0,0
	    };

        if (!(pixelFormat = ChoosePixelFormat(temphDc, &pfd))) {
            std::cout << "Failed to choose pixel format" << std::endl;
            return false;
        }
        if (!SetPixelFormat(temphDc, pixelFormat, &pfd)) {
            std::cout << "Failed to set pixel format" << std::endl;
            return false;
        }
        if (!(tempHglrc = wglCreateContext(temphDc))) {
            std::cout << "Failed to create WGL Context" << std::endl;
            return false;
        }

        if (!wglMakeCurrent(temphDc, tempHglrc)) {
            std::cout << "Failed to make the WGL Context current" << std::endl;
            return false;
        }
        return true;
    }

    bool GLPGContextImpl_Platform_WGL::CreateActualGLContext() {
        int pixelFormatsIdx[10];
        UINT pfn;

        const int pixelFormatAttribs[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0,
        };

        if (wglChoosePixelFormatARB(hDc, pixelFormatAttribs, NULL, 5, pixelFormatsIdx, &pfn) == FALSE) {
            std::cout << "Failed to choose pixel format" << std::endl;
            return false;
        }

        if (SetPixelFormat(hDc, pixelFormatsIdx[0], NULL) == FALSE) {
            std::cout << "Failed to set pixel format" << std::endl;
            return false;
        }

        const int contextAttrib[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 5,
            WGL_CONTEXT_FLAGS_ARB, 0,
            0
        };

        hGlrc = wglCreateContextAttribsARB(hDc, 0, contextAttrib);
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(tempHglrc);
        wglMakeCurrent(hDc, hGlrc);

        return true;
    }

    bool GLPGContextImpl_Platform_WGL::InitializeGLPlatformContext(uint32_t majorVersion = 4,
                                                                   uint32_t minorVersion = 5) {
        GLPGWindowImpl_Win32* windowImpl = dynamic_cast<GLPGWindowImpl_Win32*>(GLPG::GLPGWindow::GetImplInstance());
        if (!windowImpl) {
            std::cerr << "GLPG Error: Internal Failure" << std::endl;
            return false;
        }
        hDc = windowImpl->GetWin32HDC();
        temphDc = windowImpl->GetTempWin32HDC();

        if (!CreateDummyGLContext()) {
            std::cerr << "Failed to create dummy WGL Context\n";
            return false;
        }

        if (!LoadWGLFunctionPointers() || !CreateActualGLContext()) {
            std::cerr << "Failed to create actual WGL Context\n";
            return false;
        }
        LoadGLFunctionPointers();
        std::cerr << "GLPG INFO: GL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cerr << "GLPG INFO: GPU: " << glGetString(GL_RENDERER) << std::endl;
        return true;
    }

    bool GLPGContextImpl_Platform_WGL::PlatformSwapBuffers() {
        SwapBuffers(hDc);
        return true;
    }

} // namespace GLPG
