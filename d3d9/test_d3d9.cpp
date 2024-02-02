#include <iostream>

#include "../include/GLPGD3D9Context.hpp"
#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"

int main()
{
    GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;

    if (!window) {
        std::cerr << "Failed to create GLPGWindow\n";
        return -1;
    }

    GLPG::GLPGD3D9Context gc;
    if (!gc.inited) {
        std::cerr << "Failed to initialize D3D11\n";
        return -1;
    }

    if (window->CreateWindow(gc.defaultWidth, gc.defaultHeight)) {
        std::cout << "Width x Height: " << window->GetWindowWidth() << "x" << window->GetWindowHeight() << "\n";
    } else {
        std::cout << "Failed to create native window\n";
        return -1;
    }

    if (!eventLoop.InitializeEventLoop()) {
        std::cerr << "Failed to initialize event loop\n";
        return -1;
    }

    HWND *hWnd = reinterpret_cast<HWND *>(window->GetWindowHandle());
    if (!gc.CreateContext(*hWnd)) {
        std::cerr << "Failed to create gc\n";
        return -1;
    }

    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        gc.pd3d9Device->BeginScene();
        gc.pd3d9Device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 255, 0), 1.0F, 0);
        gc.pd3d9Device->EndScene();
        gc.pd3d9Device->Present(nullptr, nullptr, nullptr, nullptr);
    }
}
