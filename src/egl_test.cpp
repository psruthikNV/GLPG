#include "utils/native_window.hpp"
#include "utils/opengl_context.hpp"

int main()
{
    nativeWindow win(640, 480);
    glContext gc;

    if (!win.createNativeWindow()) {
        std::cout << "Failed to create Native Window" << std::endl;
        return -1;
    }

    if (!gc.initializeGlContext(win, 4, 6)) {
        std::cout << "Failed to initialize GL Context" << std::endl;
        return -1;
    }

    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    gc.swapBuffers();
    system("pause");
    return 0;
}
