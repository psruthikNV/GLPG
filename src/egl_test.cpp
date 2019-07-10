#ifdef _WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>

#include "utils/native_window.hpp"
#include "utils/opengl_context.hpp"

int main()
{
    nativeWindow win(640, 480);
    win.createNativeWindow();
    glContext gc;
    gc.initializeGlContext(win, 4, 6);
    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    gc.swapBuffers();
    system("pause");
    return 0;
}
