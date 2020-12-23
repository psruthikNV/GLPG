#include <iostream>
#include <vector>
#include <random>

#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam) {

    switch (message) {
        case WM_PAINT:
            return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND CreateWin32Window() {

    int a;
    int b;
    WNDCLASSEX wndClass = { 0 };

    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.hInstance = GetModuleHandle(NULL);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.lpszClassName = "D3d11 Window";

    ATOM ret = RegisterClassEx(&wndClass);

    RECT windowRect = { 0, 0, 1366, 768 };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    return (CreateWindowA(wndClass.lpszClassName, "D3d11 101",
                          WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                          windowRect.right - windowRect.left,
                          windowRect.bottom - windowRect.top, nullptr, nullptr,
                          wndClass.hInstance, nullptr));
}
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
    HWND hWindow;
    uint32_t idx = 0U;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0, 1.0);

    hWindow = CreateWin32Window();

    if (!hWindow) {
        std::cout << "Failed to create native window\n";
        return -1;
    } else {
        ShowWindow(hWindow, SW_SHOWNORMAL);
    }

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
    } else {
        std::cout << "Got pdxgi factory 2\n";
    }

    IDXGISwapChain1* pSwapchain = nullptr;

    ret = pdxgiFactory2->CreateSwapChainForHwnd(pD3d11device, hWindow, &swapchainDesc, NULL, NULL, &pSwapchain);
    if (ret != S_OK) {
        std::cout << "Swapchain creation failed due to : " << ret << "\n";
        return -1;
    } else {
        std::cout << "Created Swapchain \n";
    }
    
    ID3D11Texture2D* pbackBuffer;
    ret = pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pbackBuffer);
    if (ret != S_OK) {
        std::cout << "Failed to get back buffer\n";
        std::cout << "Err : " << ret << "\n";
        return -1;
    } else {
        std::cout << "Got back buffer\n";
    }

    ID3D11RenderTargetView* pRenderTargetView = nullptr;
    ret = pD3d11device->CreateRenderTargetView(pbackBuffer, nullptr, &pRenderTargetView);
    if (ret != S_OK) {
        std::cout << "Failed to create render target view\n";
        std::cout << "Err : " << ret << "\n";
        return -1;
    } else {
        std::cout << "Created Render target view\n";
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
    while (numFramesRendered <= maxRenderedFrames) {
        colors[0] = red.operator++();
        colors[1] = green.operator++();
        colors[2] = blue.operator++();
        pD3d11context->OMSetRenderTargets(1U, &pRenderTargetView, nullptr);
        pD3d11context->ClearRenderTargetView(pRenderTargetView, colors);
        pSwapchain->Present(1, 0);
        ++numFramesRendered;
    }

    // 2. Hello triangle

    LPCWSTR srcFilePath = L"C:/repos/d3d11_101/vtxShader.hlsl";
    LPCSTR shaderTarget = "vs_5_0";
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    std::cout << "Compiling Vertex Shader\n";
    ret = D3DCompileFromFile(srcFilePath, NULL, NULL, "main", NULL, 0, 0, &shaderBlob, &errorBlob);
    if (ret != S_OK) {
        if (ret == D3D11_ERROR_FILE_NOT_FOUND) {
            std::cout << " File not found\n";
        }
        std::cout << "Failed to compile VS\n" << std::hex << ret << "\n";
        if (errorBlob) {
            std::cout << "Error : " << (char*)errorBlob->GetBufferPointer()<< "\n";
        } else {
            std::cout << "No error blob\n";
        }
    } else {
        std::cout << "Compiled VS\n";
    }
}