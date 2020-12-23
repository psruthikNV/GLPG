#include "GLPGEvent.hpp"
#include "internal/GLPGEventImpl_Win32.hpp"

namespace GLPG {
    GLPGEventLoop::GLPGEventLoop() {
        _eventImpl = new GLPGEventImpl_Win32;
    }

    GLPGEventLoop::~GLPGEventLoop() {
        delete _eventImpl;
    }

    bool GLPGEventLoop::InitializeEventLoop() {
        return _eventImpl->CreateEventLoopImpl();
    }

    GLPGEvent GLPGEventLoop::GetEvent() {
        return _eventImpl->GetEventImpl();
    }
} // namespace GLPG
