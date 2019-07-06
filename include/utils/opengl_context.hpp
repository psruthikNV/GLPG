#include "native_window.hpp"
#ifdef __linux__
#include <EGL/egl.h>
#endif

class glContext
{
	private:
		struct glContextConfig
		{
			uint32_t glContextMinVersion;
			uint32_t glContextMajorVersion;
		};
		glContextConfig glConfig;
		struct eglResources
		{
			EGLDisplay display;
			EGLConfig config;
			EGLContext context;
			EGLSurface surface;
		};
		eglResources eglRes;
		bool loadGlFunctions();
		EGLDisplay eglDisplay;
		bool initializeEglBackend(nativeWindow &window);
	public:
		glContext();
		glContext(nativeWindow &window);
		glContext(nativeWindow &window, uint32_t minVersion, uint32_t majorVersion);
		bool initializeGlContext(nativeWindow &window, uint32_t majorVersion, uint32_t minorVersion);
		bool swapBuffers();
};
