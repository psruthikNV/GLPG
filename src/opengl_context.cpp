#include "utils/opengl_context.hpp"

bool glContext::initializeGlContext(nativeWindow &window, uint32_t majorVersion, uint32_t minorVersion)
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
    return true;
};

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

	ret = eglChooseConfig(eglRes.display, config_attrib_list, &eglRes.config, 1, &numConfig);
	if (!ret) {
		std::cout << "Failed to choose an EGLConfig for the provided configuration" << std::endl;
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

	ret = eglMakeCurrent(eglRes.display, eglRes.surface, eglRes.surface, eglRes.context);
	if (!ret) {
		std::cout << "Failed to make EGLContext current" << std::endl;
		return false;
	}

    return true;
}

bool glContext::swapBuffers()
{
#ifdef __linux__
	eglSwapBuffers(eglRes.display, eglRes.surface);
#endif
	return true;
}
glContext::glContext()
{
}
