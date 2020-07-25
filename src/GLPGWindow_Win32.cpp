#include "GLPGWindow.hpp"
#include "internal/GLPGWindowImpl_Win32.hpp"

GLPGWindow* GLPGWindow::_instance = nullptr;
namespace GLPG {

    GLPGWindow::GLPGWindow() {
        if (!_windowImpl) {
            _windowImpl = new GLPGWindowImpl_Win32;
        }
    }

    GLPGWindow::~GLPGWindow() {
        if (_windowImpl) {
            delete _windowImpl;
        }
        if (_instance) {
            delete _instance;
        }
    }

    GLPGWindow* GLPGWindow::GetInstance() {
        if (!_instance) {
            _instance = new GLPGWindow;
        }
        return _instance;
    }

    GLPGWindowImpl* GLPGWindow::GetImplInstance() {
        return _windowImpl;
    }

    bool GLPGWindow::CreateWindow(uint32_t width, uint32_t height) {
        return windowImpl->CreateWindowImpl(width, height);
    }

    uint32_t GLPGWindow::GetWindowWidth() {
        return windowImpl->GetWidthImpl();
    }

    uint32_t GLPGWindow::GetWindowHeight() {
        return windowImpl->GetHeightImpl();
    }

} // namespace GLPG
