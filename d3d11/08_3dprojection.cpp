#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <cstring>

#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"
#include "../include/math/GLPGMath.hpp"

#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#undef CreateWindow // We require this undef since GLPG's CreateWindow collides with Win32's CreateWindow macro


// These vertices are copied from the OpenGL projection
// matrix example (05_projection_matrix.cpp) and require the following
// WARs to work with D3D :
// 1. Negation of Z coordinate to convert the coordinates to LH
// 2. Disabling of culling since D3D considers CCW faces to be back facing by default
//    and culls them.
float vertexData[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

void ConvertVertexDataToLH() {
    // TODO: Is this even required?
    for (uint32_t idx = 0U; idx < sizeof(vertexData) / sizeof(float); idx++) {
        if ((idx + 1) % 3 == 0) {
            vertexData[idx] = -vertexData[idx];
        }
    }
}

int main() {
    ConvertVertexDataToLH();
    HRESULT ret;
    uint32_t idx = 0U;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0, 1.0);
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;

    GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    if (!window) {
        std::cerr << "Failed to create GLPGWindow\n";
        return -1;
    }

    int32_t width = 2560;
    int32_t height = 1440;
    if (window->CreateWindow(width, height)) {
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
            << "GB\n\n";
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
    }

    ret = debugInfoQueue->PushEmptyStorageFilter();
    if (ret != S_OK) {
        std::cerr << "Failed to push empty filter onto info queue\n";
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

    // Load compiled Vertex shader binary
    char *data;
    char *psData;
    const char *compiledVSPath = "07_projection_matrix_vs";
    const char *compiledPSPath = "07_projection_matrix_ps";
    std::fstream fs;
    std::streampos size;
    std::streampos psSize;
    ID3D11VertexShader *ppVertexShader;
    ID3D11PixelShader *ppPixelShader;
    fs.open(compiledVSPath, std::ios::in | std::ios::binary | std::ios::ate);

    if (!fs.is_open()) {
        std::cerr << "Failed to open vso file\n";
    } else {
        size = fs.tellg();
        data = new char[size];
        fs.seekg(0, std::ios::beg);
        fs.read((char *)(data), size);
        fs.close();
    }

    fs.open(compiledPSPath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!fs.is_open()) {
        std::cerr << "Failed to open vso file\n";
    } else {
        psSize = fs.tellg();
        psData = new char[psSize];
        fs.seekg(0, std::ios::beg);
        fs.read((char *)(psData), psSize);
        fs.close();
    }

    ret = pD3d11device->CreateVertexShader(static_cast<void *>(data), size, nullptr, &ppVertexShader);
    if (ret != S_OK) {
        std::cerr << "Failed to create vertex shader\n";
        std::cerr << "Ret: " << std::hex << ret << std::dec << "\n";
    }

    ret = pD3d11device->CreatePixelShader(static_cast<void *>(psData), psSize, nullptr, &ppPixelShader);
    if (ret != S_OK) {
        std::cerr << "Failed to create pixel shader\n";
        std::cerr << "Ret: " << std::hex << ret << std::dec << "\n";
    }
    
    D3D11_INPUT_ELEMENT_DESC iaDesc = {
        "POSITION",
        0,
        DXGI_FORMAT_R32G32B32_FLOAT,
        0,
        0,
        D3D11_INPUT_PER_VERTEX_DATA,
        0
    };

    ID3D11InputLayout *pInputLayout;
    ret = pD3d11device->CreateInputLayout(&iaDesc, 1U, data, size, &pInputLayout);
    if (ret != S_OK) {
        std::cerr << "Failed to create input layout\n";
        std::cerr << "Ret: " << std::hex << ret << std::dec << "\n";
    }

    D3D11_BUFFER_DESC vtxBufferDesc = {
        sizeof(vertexData),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_VERTEX_BUFFER,
        0,
        0,
        0
    };

    D3D11_SUBRESOURCE_DATA initData = {
        vertexData,
        0,
        0
    };

    ID3D11Buffer *pVtxBuffer;
    ret = pD3d11device->CreateBuffer(&vtxBufferDesc, &initData, &pVtxBuffer);
    if (ret != S_OK) {
        std::cerr << "Failed to create Vertex Buffer\n";
        std::cerr << "Ret: " << std::hex << ret << std::dec << "\n";
    }
    
    UINT stride = sizeof(VtxInput);
    UINT offset = 0;

    RECT winRect;
    GetClientRect(*hWindow, &winRect);
    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)(winRect.right - winRect.left), (FLOAT)(winRect.bottom - winRect.top), 0.0f, 1.0f };

    GLPG::vec3_f translateVector = {0.0, 0.0, 0.0};
    GLPG::vec3_f upVector = {0.0, 1.0, 0.0};
    GLPG::vec3_f eyePosition = {0.0, 0.0, -5.0};
    GLPG::vec3_f viewVector = {0.0, 0.0, 1.0};
    struct MVP {
        GLPG::mat4x4_f model;
        GLPG::mat4x4_f view;
        GLPG::mat4x4_f projection;
    };
    MVP mvp{};
    mvp.model = translate(mvp.model, translateVector);
    mvp.view = lookAtLH(eyePosition, viewVector, upVector);
    mvp.projection = GLPG::gluPerspectiveLH(45.0F, 2560.0F / 1440.0F, -0.01F, -100.0F);

    D3D11_BUFFER_DESC constantBufferDesc = {
        sizeof(MVP),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_CONSTANT_BUFFER,
        0,
        0,
        0
    };

    D3D11_SUBRESOURCE_DATA constantBufferInitData = {
        &mvp,
        0,
        0
    };

    ID3D11Buffer *pVtxConstantBuffer;

    ret = pD3d11device->CreateBuffer(&constantBufferDesc, &constantBufferInitData, &pVtxConstantBuffer);
    if (ret != S_OK) {
        std::cerr << "Failed to create constant buffer\n";
        return -1;
    }

    D3D11_RASTERIZER_DESC rasterizerDesc = {
        D3D11_FILL_SOLID,
        D3D11_CULL_NONE,
        FALSE,
        0,
        0.0F,
        0.0F,
        TRUE,
        FALSE,
        FALSE,
        FALSE
    };
    ID3D11RasterizerState *pRasterizerState;
    ret = pD3d11device->CreateRasterizerState(&rasterizerDesc, &pRasterizerState);
    if (ret != S_OK) {
        std::cerr << "Failed to create RS\n";
        return -1;
    }

    pD3d11context->RSSetState(pRasterizerState); // To disable back face culling
    pD3d11context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pD3d11context->IASetInputLayout(pInputLayout);
    pD3d11context->IASetVertexBuffers(0, 1, &pVtxBuffer, &stride, &offset);

    pD3d11context->VSSetShader(ppVertexShader, NULL, 0);
    pD3d11context->PSSetShader(ppPixelShader, NULL, 0);

    pD3d11context->RSSetViewports(1, &viewport);
    pD3d11context->OMSetRenderTargets(1U, &pRenderTargetView, nullptr);
    pD3d11context->VSSetConstantBuffers(0, 1, &pVtxConstantBuffer);

    float colors[4];
    colors[3] = 1.0F;
    colors[0] = 0.0F;
    colors[1] = 0.5F;
    colors[2] = 0.5F;
    

    const float radius = 20.0F;
    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        SYSTEMTIME sysTime;
        GetSystemTime(&sysTime);

        float camX = sin(sysTime.wMilliseconds) * radius;
        float camZ = cos(sysTime.wMilliseconds) * radius;
        GLPG::vec3_f newEyePos = {camX, 0.0F, camZ};
        mvp.view = lookAtLH(newEyePos, viewVector, upVector);
        pD3d11context->UpdateSubresource(pVtxConstantBuffer, 0, 0, &mvp, 0, 0);
        pD3d11context->ClearRenderTargetView(pRenderTargetView, colors);
        pD3d11context->Draw(36, 0);
        pSwapchain->Present(0, 0);
        Sleep(16.6);
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