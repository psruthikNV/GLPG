#include <iostream>
#include <vector>
#include <random>
#include <fstream>

#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"

#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
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

    if (window->CreateWindow(640, 480)) {
        std::cout << "Width x Height: " << window->GetWindowWidth() << "x" << window->GetWindowHeight() << "\n";
    } else {
        std::cout << "Failed to create native window\n";
        return -1;
    }

    if (!eventLoop.InitializeEventLoop()) {
        std::cerr << "Failed to initialize event loop\n";
        return -1;
    }

    HWND *hWindow = reinterpret_cast<HWND *>(window->GetWindowHandle());

    IDXGIFactory* pdxgiFactory = nullptr;
    ret = CreateDXGIFactory(__uuidof(IDXGIFactory),
                            reinterpret_cast<void**>(&pdxgiFactory));

    if (ret) {
        std::cout << "Failed to create DXGIFactory instance\n";
        return -1;
    }

    uint32_t numAdapters = 0;
    IDXGIAdapter* pAdapter;
    std::vector<IDXGIAdapter*> adapters;

    while (pdxgiFactory->EnumAdapters(numAdapters, &pAdapter) !=
           DXGI_ERROR_NOT_FOUND) {
        adapters.push_back(pAdapter);
        ++numAdapters;
    }

    DXGI_ADAPTER_DESC pdesc;
    uint32_t adapterIdxInUse = 0U;
    uint32_t currentMaxVidmem = 0U;
    for (uint32_t idx = 0U; idx < numAdapters; idx++) {
        ret = adapters[idx]->GetDesc(&pdesc);
        std::cout << "Details of Adapter " << idx + 1 << "\n\n";
        std::wcout << "Description : " << pdesc.Description << "\n";
        std::cout << "Vidmem : " << pdesc.DedicatedVideoMemory / 1000000000 << "GB\n";
        std::cout << "Dedicated Sysmem : "
            << pdesc.DedicatedSystemMemory / 1000000000 << "GB\n";
        std::cout << "Shared Sysmem : " << pdesc.SharedSystemMemory / 1000000000
            << "GB\n";
        if (pdesc.DedicatedVideoMemory > 0 &&
            pdesc.DedicatedVideoMemory > currentMaxVidmem) {
            adapterIdxInUse = idx;
            currentMaxVidmem = pdesc.DedicatedSystemMemory;
        }
    }

    ID3D11Device* pD3d11device;
    ID3D11DeviceContext* pD3d11context;

    ret = D3D11CreateDevice(adapters[adapterIdxInUse], D3D_DRIVER_TYPE_UNKNOWN,
                            NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL,
                            D3D11_SDK_VERSION, &pD3d11device, NULL, &pD3d11context);

    if (ret != S_OK) {
        std::cout << "Failed to Create D3d11 device\n";
        std::cout << "Err:" << ret << "\n";
        return -1;
    }

    ID3D11InfoQueue* debugInfoQueue;
    ret = pD3d11device->QueryInterface(__uuidof(ID3D11InfoQueue), (void **)&debugInfoQueue);
    if (ret != S_OK) {
        std::cerr << "Failed to query Info Queue\n";
        return -1;
    } else {
        std::cerr << "Queried Info queue\n";
    }

    ret = debugInfoQueue->PushEmptyStorageFilter();
    if (ret != S_OK) {
        std::cerr << "Failed to push empty filter onto info queue\n";
        return -1;
    } else {
        std::cerr << "Pushed filter\n";
    }


    
    DXGI_MODE_DESC modeDesc = { 0 };
    modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    modeDesc.Width = 0;
    modeDesc.Height = 0;
    modeDesc.RefreshRate = DXGI_RATIONAL{ 60, 1 };
    modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;

    DXGI_SAMPLE_DESC sampleDesc = { 0 };
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    DXGI_SWAP_CHAIN_DESC1 swapchainDesc = { 0 };
    swapchainDesc.BufferCount = 1;
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY;
    swapchainDesc.SampleDesc = sampleDesc;
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchainDesc.Stereo = FALSE;
    swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    //swapchainDesc.Scaling = DXGI_SCALING_NONE;

    IDXGIFactory2* pdxgiFactory2;
    adapters[adapterIdxInUse]->GetParent(__uuidof(IDXGIFactory2), (void**)& pdxgiFactory2);

    if (!pdxgiFactory2) {
        std::cout << "Failed to get pdxgifactory 2\n";
        return -1;
    }

    IDXGISwapChain1* pSwapchain = nullptr;

    ret = pdxgiFactory2->CreateSwapChainForHwnd(pD3d11device, *hWindow, &swapchainDesc, NULL, NULL, &pSwapchain);
    if (ret != S_OK) {
        std::cout << "Swapchain creation failed due to : " << ret << "\n";
        return -1;
    }
    
    ID3D11Texture2D* pbackBuffer;
    ret = pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pbackBuffer);
    if (ret != S_OK) {
        std::cout << "Failed to get back buffer\n";
        std::cout << "Err : " << ret << "\n";
        return -1;
    }

    ID3D11RenderTargetView* pRenderTargetView = nullptr;
    ret = pD3d11device->CreateRenderTargetView(pbackBuffer, nullptr, &pRenderTargetView);
    if (ret != S_OK) {
        std::cout << "Failed to create render target view\n";
        std::cout << "Err : " << ret << "\n";
        return -1;
    }

    pD3d11context->OMSetRenderTargets(1U, &pRenderTargetView, nullptr);

    float colors[4];
    colors[3] = 0.0F;

    counter_f red(dist(gen));
    counter_f green(dist(gen));
    counter_f blue(dist(gen));

    // 1. Clear color
    uint32_t numFramesRendered = 0U;
    uint32_t maxRenderedFrames = 120U;
    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        colors[0] = red.operator++();
        colors[1] = green.operator++();
        colors[2] = blue.operator++();
        pD3d11context->OMSetRenderTargets(1U, &pRenderTargetView, nullptr);
        pD3d11context->ClearRenderTargetView(pRenderTargetView, colors);
        pSwapchain->Present(1, 0);
    }

    // Below code is taken from https://stackoverflow.com/a/57362700
    // TODO: Add a debug thread that polls GetNumStorageMessages
    uint64_t messageCount = debugInfoQueue->GetNumStoredMessages();
    for (uint64_t idx = 0U; idx < messageCount; idx++) {
        SIZE_T message_size = 0;
        debugInfoQueue->GetMessage(idx, nullptr, &message_size); //get the size of the message

        D3D11_MESSAGE* message = (D3D11_MESSAGE*) malloc(message_size); //allocate enough space
        ret = debugInfoQueue->GetMessage(idx, message, &message_size);
        if (ret != S_OK) {
            std::cerr << "Failed to get any message\n";
        } else {
            std::cerr << "D3D11 Debug: " << message->DescriptionByteLength << " " << message->pDescription << "\n";
        }
        free(message);
    }
    debugInfoQueue->ClearStoredMessages();
}