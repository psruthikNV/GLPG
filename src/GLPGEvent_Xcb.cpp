#include "GLPGEvent.hpp"
#include "internal/GLPGEventImpl_Xcb.hpp"

namespace GLPG {
    GLPGEventLoop::GLPGEventLoop() {
        _eventImpl = new GLPGEventImpl_Xcb;
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
