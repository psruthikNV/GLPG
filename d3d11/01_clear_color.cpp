#include <iostream>
#include <vector>
#include <random>
#include <fstream>

#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"
#include "../include/GLPGD3D11Context.hpp"

#undef CreateWindow // We require this undef since GLPG's CreateWindow collides with Win32's CreateWindow macro

template <typename T>
class counter {
public:
    counter(T val) : m_val(val), performAddition(true) {};
    counter() : m_val(T(0)), performAddition(true) {};

    // Perhaps the most stupidest op overload ever.
    T operator++() {
        if (performAddition) {
            m_val += 0.001;
        } else {
            m_val -= 0.001;
        }

        if (m_val > T(1)|| m_val <= T(0)) {
            performAddition = !performAddition;
        }
        return m_val;
    }

private:
    T m_val;
    bool performAddition;
};

using counter_f = counter<float>;

int main() {
    HRESULT ret;
    uint32_t idx = 0U;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;

    GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    if (!window) {
        std::cerr << "Failed to create GLPGWindow\n";
        return -1;
    }

    GLPG::GLPGD3D11Context gc;
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

    if (!gc.InitializeSwapchain()) {
        std::cerr << "Failed to initialize swapchain\n";
        return -1;
    }

    if (!eventLoop.InitializeEventLoop()) {
        std::cerr << "Failed to initialize event loop\n";
        return -1;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pswapchainBuffers = {};
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView = {};
    for (uint32_t idx = 0; idx < 1; idx++) {
        ret = gc.pSwapchain->GetBuffer(idx, __uuidof(ID3D11Texture2D), (void**)&pswapchainBuffers);
         if (ret != S_OK) {
            std::cerr << "Failed to get buffer. Ret: " << std::hex << ret << std::dec << "\n";
            return -1;
        } else {
            if (ret = gc.pD3d11Device->CreateRenderTargetView(pswapchainBuffers.Get(), nullptr, pRenderTargetView.GetAddressOf());
                ret != S_OK) {
                std::cout << "Failed to get back buffer\n";
                std::cout << "Err : " << ret << "\n";
                return -1;
            }
        }
    }

    gc.pD3d11Context->OMSetRenderTargets(1U, pRenderTargetView.GetAddressOf(), nullptr);

    float colors[4];
    colors[3] = 0.0F;

    counter_f red(dist(gen));
    counter_f green(dist(gen));
    counter_f blue(dist(gen));

    uint32_t numFramesRendered = 0U;
    uint32_t maxRenderedFrames = 120U;
    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        colors[0] = red.operator++();
        colors[1] = green.operator++();
        colors[2] = blue.operator++();
        gc.pD3d11Context->OMSetRenderTargets(1U, pRenderTargetView.GetAddressOf(), nullptr);
        gc.pD3d11Context->ClearRenderTargetView(pRenderTargetView.Get(), colors);
        gc.pSwapchain->Present(0, 0);
    }
}
