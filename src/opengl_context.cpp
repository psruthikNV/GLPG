#include "utils/opengl_context.hpp"

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

void* glContext::loadGLFunction(const char *name) {
#ifdef _WIN32
    void *f = (void *)wglGetProcAddress(name);

    if (!f || (f == (void *)0x1) || (f == (void *)0x3)
        || (f == (void *)-1)) {
        std::cout << "Can't find Addresso of GL Function : " << name << " in ICD"
                  << std::endl; 
        OutputDebugString("\n Cant find ProcAddress in ICD.");
        HMODULE module = LoadLibrary("opengl32.dll");
        f = (void *)GetProcAddress(module, name);

        if (!f)
            return NULL;
    }
    return f;
#elif defined __linux__
    void *f = (void *)eglGetProcAddress(name);
    if (!f) {
        std::cout << "Failed to load GL Function : " << name << std::endl;
        return NULL;
    }
    return f;
#endif
}

void glContext::initializeGLFunctionPointers()
{
    glGenBuffers = (PFNGLGENBUFFERSPROC)loadGLFunction("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)loadGLFunction("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)loadGLFunction("glBufferData");
    glGetStringi = (PFNGLGETSTRINGIPROC)loadGLFunction("glGetStringi");
    glCreateShader = (PFNGLCREATESHADERPROC)loadGLFunction("glCreateShader");
    glDeleteShader = (PFNGLDELETESHADERPROC)loadGLFunction("glDeleteShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)loadGLFunction("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)loadGLFunction("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)loadGLFunction("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)loadGLFunction("glGetShaderInfoLog");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)loadGLFunction("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)loadGLFunction("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)loadGLFunction("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)loadGLFunction("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)loadGLFunction("glGetProgramInfoLog");
    glUseProgram = (PFNGLUSEPROGRAMPROC)loadGLFunction("glUseProgram");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)loadGLFunction("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)loadGLFunction("glBindVertexArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)loadGLFunction("glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)loadGLFunction("glEnableVertexAttribArray");
    glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC)loadGLFunction("glVertexArrayAttribBinding");
    glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC)loadGLFunction("glVertexArrayVertexBuffer");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)loadGLFunction("glGetUniformLocation");
    glUniform4f = (PFNGLUNIFORM4FPROC)loadGLFunction("glUniform4f");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)loadGLFunction("glUniformMatrix4fv");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)loadGLFunction("glGenerateMipmap");
}

bool glContext::initializeGlContext(nativeWindow &window, uint32_t majorVersion,
                                    uint32_t minorVersion)
{
    bool ret;

    glConfig.glContextMajorVersion = majorVersion;
    glConfig.glContextMinVersion = minorVersion;

#ifdef __linux__
    ret = initializeEglBackend(window);
#elif defined _WIN32
    ret = initializeWglBackend(window);
#endif
    if (!ret) {
        std::cout << "Failed to initialize OpenGL Windowing API" << std::endl;
        return false;
    }
    initializeGLFunctionPointers();
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
    //std::cout << "OpenGL Extensions: " << glGetString(GL_EXTENSIONS) << std::endl;

    return true;
};

#ifdef __linux__
bool glContext::initializeEglBackend(nativeWindow &window)
{
    EGLBoolean ret;
    EGLint numConfig;
    static EGLint const config_attrib_list[] = {
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_NONE
    };
    static EGLint const context_attrib_list[] = {
        EGL_CONTEXT_MAJOR_VERSION, static_cast<EGLint>(glConfig.glContextMajorVersion),
        EGL_CONTEXT_MINOR_VERSION, static_cast<EGLint>(glConfig.glContextMinVersion),
        EGL_NONE
    };

    eglRes.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglRes.display == EGL_NO_DISPLAY) {
        std::cout << "Failed to create EGL Display" << std::endl;
    }

    ret = eglInitialize(eglRes.display, NULL, NULL);
    if (!ret) {
        std::cout << "Failed to initialize EGLDisplay" << std::endl;
        return false;
    }

    ret = eglChooseConfig(eglRes.display, config_attrib_list,
                          &eglRes.config, 1, &numConfig);
    if (!ret) {
        std::cout << "Failed to choose an EGLConfig for"
                     "the provided configuration" << std::endl;
        return false;
    }

    if ((eglQueryAPI() != EGL_OPENGL_API) && !(eglBindAPI(EGL_OPENGL_API))) {
        std::cout << "Failed to bind OpenGL to EGL" << std::endl;
        return false;
    }

    eglRes.context = eglCreateContext(eglRes.display, eglRes.config,
            EGL_NO_CONTEXT, context_attrib_list);
    if (eglRes.context == EGL_NO_CONTEXT) {
        std::cout << "Failed to create EGLContext" << std::endl;
        return false;
    }

    eglRes.surface = eglCreateWindowSurface(eglRes.display, eglRes.config,
            window.getNativeHandle(),
            NULL);
    if (eglRes.surface == EGL_NO_SURFACE) {
        std::cout << "Failed to create EGLSurface" << std::endl;
        return false;
    }

    ret = eglMakeCurrent(eglRes.display, eglRes.surface,
                         eglRes.surface, eglRes.context);
    if (!ret) {
        std::cout << "Failed to make EGLContext current" << std::endl;
        return false;
    }

    return true;
}
#elif defined _WIN32
bool glContext::createDummyGlContext()
{
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

	if (!(wglRes.pixelFormat = ChoosePixelFormat(wglRes.hDc, &pfd))) {
        std::cout << "Failed to choose pixel format" << std::endl;
        return false;
    }
	if (!SetPixelFormat(wglRes.hDc, wglRes.pixelFormat, &pfd)) {
        std::cout << "Failed to set pixel format" << std::endl;
        return false;
    }
	if (!(wglRes.tempHglrc = wglCreateContext(wglRes.hDc))) {
        std::cout << "Failed to create WGL Context" << std::endl;
        return false;
    }

	if (!wglMakeCurrent(wglRes.hDc, wglRes.tempHglrc)) {
        std::cout << "Failed to make the WGL Context current" << std::endl;
        return false;
    }

    return true;
}

/*
bool glContext::createActualGlContext()
{
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
		WGL_CONTEXT_MINOR_VERSION_ARB, 5, // Use 4.5 instead of 4.6 due to outdated Intel gen9 drivers
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	hGlrc = wglCreateContextAttribsARB(hDc, 0, contextAttrib);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(temp_hGlrc);
	wglMakeCurrent(hDc, hGlrc);

    return false;
}
*/
bool glContext::initializeWglBackend(nativeWindow &window)
{
    wglRes.hDc = window.getNativeHandle();
    if (!createDummyGlContext()) {
        std::cout << "Failed to create WGL Context" << std::endl;
        return false;
    }/*
    else if (!createActualGlContext()) {
        std::cout << "Failed to create actual WGL Contxt" << std::endl;
        return false;
    }*/

    return true;
}
#endif

bool glContext::swapBuffers()
{
#ifdef __linux__
    eglSwapBuffers(eglRes.display, eglRes.surface);
#elif defined _WIN32
    SwapBuffers(wglRes.hDc);
#endif
    return true;
}
glContext::glContext()
{
}
