#include <iostream>

#include "../include/GLPGD3D8Context.hpp"
#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"

struct VertexFormat
{
    float x;
    float y;
    float z;
    float rhw; // 1/w
    uint32_t color;
};

VertexFormat vertexData[3] {
    // 2560x1600
    // 1280x800
    { 1280.0F, 50.0F, 0.5F, 1.0F, 0xffff0000 },
    { 2000.0F, 1000.0F, 0.5F, 1.0F, 0xff00ff00 },
    { 500.0F, 1000.0F, 0.5F, 1.0F, 0xff00ffff }
};

int main()
{
    GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;

    if (!window) {
        std::cerr << "Failed to create GLPGWindow\n";
        return -1;
    }

    GLPG::GLPGD3D8Context gc;
    if (!gc.inited) {
        std::cerr << "Failed to initialize D3D8\n";
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

    // Create a vertex buffer
    IDirect3DVertexBuffer8 *vertexBuffer { };
    if (gc.pd3d8Device->CreateVertexBuffer(sizeof(vertexData), 0, D3DFVF_XYZRHW | D3DFVF_DIFFUSE,
                                           D3DPOOL_DEFAULT, &vertexBuffer) != D3D_OK) {
        std::cerr << "Failed to create vertex buffer\n";
        return -1;
    }

    void *vertexBufferPointer { };
    if (vertexBuffer->Lock(0, sizeof(vertexData), reinterpret_cast<BYTE **>(&vertexBufferPointer), 0) == D3D_OK) {
        std::memcpy(vertexBufferPointer, vertexData, sizeof(vertexData));
        vertexBuffer->Unlock();
    } else {
        std::cerr << "Failed to lock the vertex buffer for copy\n";
        return -1;
    }

    uint8_t red { 0U };
    uint8_t green { 255U };
    uint8_t blue { 0U };
    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        gc.pd3d8Device->BeginScene();
        gc.pd3d8Device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(red, green, ++blue), 1.0F, 0);
        gc.pd3d8Device->SetStreamSource(0, vertexBuffer, sizeof(VertexFormat));
        gc.pd3d8Device->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
        gc.pd3d8Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
        gc.pd3d8Device->EndScene();
        gc.pd3d8Device->Present(nullptr, nullptr, nullptr, nullptr);
    }
}
