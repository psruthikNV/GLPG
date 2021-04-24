#include <iostream>
#include <vector>
#include <random>
#include <fstream>

#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#undef CreateWindow // We require this since GLPG::CreateWindow collides with the Win32 API

template <typename T>
class counter {
public:
    counter(T val) : m_val(val), performAddition(true) {};
    counter() : m_val(T(0)), performAddition(true) {};

    // Perhaps the most stupidest op overload ever.
    T operator++() {
        if (performAddition) {
            m_val += T(0.001);
        } else {
            m_val -= T(0.001);
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
using Microsoft::WRL::ComPtr;

int main() {
    HRESULT ret;
    uint32_t idx = 0U;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;
    counter_f red(dist(gen));
    counter_f green(dist(gen));
    counter_f blue(dist(gen));
    uint32_t width = 2560U;

    GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    uint32_t height = 1440U;
    if (!window) {
        std::cerr << "Failed to create GLPGWindow\n";
        return -1;
    }

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
    if (!hWindow) {
        std::cerr << "Failed to get handle to native window\n";
    }

    ComPtr<ID3D12Debug> pdebugInterface;
    if (D3D12GetDebugInterface(IID_PPV_ARGS(&pdebugInterface)) != S_OK) {
        std::cerr << "Failed to get D3d12 debug interface\n";
        return -1;
    } else {
        pdebugInterface->EnableDebugLayer();
    }

    ComPtr<IDXGIFactory> pdxgiFactory = nullptr;
    ret = CreateDXGIFactory(IID_PPV_ARGS(&pdxgiFactory));

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

    uint32_t numOutputs = 0U;
    IDXGIOutput* pOutput;
    std::vector<IDXGIOutput*> vOutputs;
    while(adapters[adapterIdxInUse]->EnumOutputs(numOutputs, &pOutput) != 
          DXGI_ERROR_NOT_FOUND) {
        vOutputs.push_back(pOutput);
        ++numOutputs;
    }


    for (uint32_t idx = 0U; idx < numOutputs; idx++) {
        DXGI_OUTPUT_DESC outputDesc = {};
        if (vOutputs[idx]->GetDesc(&outputDesc) == S_OK) {
            std::cout << "Name: " << outputDesc.DeviceName << "\n";
            std::cout << "Rect Left: " << outputDesc.DesktopCoordinates.left << " Right: " << outputDesc.DesktopCoordinates.right << "\n";
            std::cout << "Rect Top: " << outputDesc.DesktopCoordinates.top << " Bottom: " << outputDesc.DesktopCoordinates.bottom << "\n";
        }

    }

    ComPtr<ID3D12Device> pD3d12Device;

    if (D3D12CreateDevice(adapters[adapterIdxInUse], D3D_FEATURE_LEVEL_12_0,
                          IID_PPV_ARGS(&pD3d12Device)) != S_OK) {
        std::cerr << "Failed to create D3d12 device\n";
    }

    ComPtr<ID3D12InfoQueue> pdebugInfoQueue;
    ret = pD3d12Device->QueryInterface(__uuidof(ID3D12InfoQueue),
          reinterpret_cast<void **>(pdebugInfoQueue.GetAddressOf()));
    if (ret != S_OK) {
        std::cerr << "Failed to query Info Queue\n";
        return -1;
    }

    ret = pdebugInfoQueue->PushEmptyStorageFilter();
    if (ret != S_OK) {
        std::cerr << "Failed to push empty filter onto info queue\n";
        return -1;
    }

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        D3D12_COMMAND_QUEUE_FLAG_NONE,
        0U
    };
    ComPtr<ID3D12CommandQueue> pCommandQueue;

    if (pD3d12Device->CreateCommandQueue(&commandQueueDesc, 
                                         IID_PPV_ARGS(&pCommandQueue)) != S_OK) {
        std::cerr << "Failed to create command queue\n";
        return -1;
    }

    DXGI_SAMPLE_DESC sampleDesc = { 0 };
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;


    DXGI_SWAP_CHAIN_DESC1 swapchainDesc = { 0 };
    swapchainDesc.BufferCount = 2; // BufferCount must be > 1 for DXGI_SWAP_EFFECT_FLIP_DISCARD
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY;
    swapchainDesc.SampleDesc = sampleDesc;
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // D3D12 does not support blitting swap effects and only flip modes must be used
    swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchainDesc.Stereo = FALSE;
    swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;   

    ComPtr<IDXGIFactory2> pdxgiFactory2;

    adapters[adapterIdxInUse]->GetParent(IID_PPV_ARGS(&pdxgiFactory2));
    if (!pdxgiFactory2) {
        std::cout << "Failed to get pdxgifactory 2\n";
        return -1;
    }

    ComPtr<IDXGISwapChain1> pSwapchain;
    ret = pdxgiFactory2->CreateSwapChainForHwnd(pCommandQueue.Get(), *hWindow, &swapchainDesc, NULL, NULL, &pSwapchain);
    if (ret != S_OK) {
        std::cout << "Swapchain creation failed due to : " << std::hex << ret << std::dec << "\n";
        return -1;
    }

    ComPtr<IDXGISwapChain3> pSwapchain3;
    ret = pSwapchain->QueryInterface(IID_PPV_ARGS(&pSwapchain3));
    if (ret != S_OK) {
        std::cerr << "Failed to query IDXGISwapChain3\n";
    }

    uint32_t backBufferIdx = pSwapchain3->GetCurrentBackBufferIndex();

    D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeap = {};
    rtvDescHeap.NumDescriptors = 2;
    rtvDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ID3D12DescriptorHeap *pRtvDescHeap;
    ret = pD3d12Device->CreateDescriptorHeap(&rtvDescHeap, __uuidof(ID3D12DescriptorHeap), (void **)&pRtvDescHeap);
    if (ret != S_OK) {
        std::cerr << "Failed to create RTV Descriptor heap\n";
    }

    D3D12_CPU_DESCRIPTOR_HANDLE rtvDescHandle = pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
    uint32_t rtvDescIncrementSize = pD3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    ComPtr<ID3D12Resource> pswapchainBuffers[2];

    for (uint32_t idx = 0; idx < 2; idx++) {
        ret = pSwapchain3->GetBuffer(idx, IID_PPV_ARGS(&pswapchainBuffers[idx]));
        if (ret != S_OK) {
            std::cerr << "Failed to get buffer\n";
        } else {
            pD3d12Device->CreateRenderTargetView(pswapchainBuffers[idx].Get(), nullptr, rtvDescHandle);
            rtvDescHandle.ptr += rtvDescIncrementSize;
        }
    }
    if (ret != S_OK) {
        std::cerr << "Failed to create RTVs for swapchain buffers\n";
        return -1;
    }

    ComPtr<ID3D12CommandAllocator> pCmdAllocator;
    ret = pD3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCmdAllocator));
    if (ret != S_OK) {
        std::cerr << "Failed to create command allocator\n";
        return -1;
    }

    ComPtr<ID3D12GraphicsCommandList> pCmdList;
    ret = pD3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                          pCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&pCmdList));
    if (ret != S_OK) {
        std::cerr << "Failed to create command list\n";
        return -1;
    }

    ret = pCmdList->Close();
    if (ret != S_OK) {
        std::cerr << "Failed to close command list\n";
        return -1;
    }

    ComPtr<ID3D12Fence> pFence;
    uint32_t initialFenceValue = 0U;
    uint32_t signaledFenceValue = initialFenceValue + 1U;
    ret = pD3d12Device->CreateFence(initialFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
    if (ret != S_OK) {
        std::cerr << "Failed to create D3d12 fence\n";
        return -1;
    }

    // Create an unsignaled event
    HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fenceEvent == nullptr) {
        std::cerr << "Failed to create event\n";
        return -1;
    }

    float colors[4];
    colors[3] = 0.0F;

    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
	    colors[0] = red.operator++();
	    colors[1] = green.operator++();
	    colors[2] = blue.operator++();

        ret = pCmdAllocator->Reset();
        if (ret != S_OK) {
            std::cerr << "Failed to reset command allocator: " << std::hex << ret << std::dec << "\n" ;
            goto fail;
        }

        ret = pCmdList->Reset(pCmdAllocator.Get(), nullptr);
        if (ret != S_OK) {
            std::cerr << "failed to reset command list\n";
            goto fail;
        }

        D3D12_RESOURCE_TRANSITION_BARRIER rtvPreTransationBarrier = {
            pswapchainBuffers[backBufferIdx].Get(),
            D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        };

        D3D12_RESOURCE_BARRIER rtvPreBarrier = {};
        rtvPreBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        rtvPreBarrier.Transition = rtvPreTransationBarrier;
        rtvPreBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

        pCmdList->ResourceBarrier(1, &rtvPreBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE currentRtvHandle = pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        currentRtvHandle.ptr += (backBufferIdx * rtvDescIncrementSize);

        pCmdList->ClearRenderTargetView(currentRtvHandle, colors, 0, nullptr);

        D3D12_RESOURCE_TRANSITION_BARRIER rtvPostTransationBarrier = {
            pswapchainBuffers[backBufferIdx].Get(),
            D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
        };
    
        D3D12_RESOURCE_BARRIER rtvPostBarrier = {};
        rtvPostBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        rtvPostBarrier.Transition = rtvPostTransationBarrier;
        rtvPostBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

        pCmdList->ResourceBarrier(1, &rtvPostBarrier);

        if ((ret = pCmdList->Close()) != S_OK) {
            std::cerr << "Failed to close command list " << std::hex << ret << std::dec << "\n" ;
            goto fail;
        }

        ID3D12CommandList *ppCmdList;
        ret = pCmdList->QueryInterface(IID_PPV_ARGS(&ppCmdList));
        if (ret != S_OK) {
            std::cerr << "Failed to get cmdlist\n";
            goto fail;
        }
        pCommandQueue->ExecuteCommandLists(1U, &ppCmdList);

        ret = pSwapchain3->Present(0, 0);
        if (ret != S_OK) {
            std::cerr << "Failed to present \n";
            goto fail;
        }

        ret = pCommandQueue->Signal(pFence.Get(), signaledFenceValue);
        if (ret != S_OK) {
            std::cerr << "Failed to signal fence\n";
            goto fail;
        }

        if (pFence->GetCompletedValue() < signaledFenceValue) {
            if (pFence->SetEventOnCompletion(signaledFenceValue, fenceEvent) != S_OK) {
                std::cerr << "failed to set event on completion\n";
                goto fail;
            }
            WaitForSingleObject(fenceEvent, INFINITE);
        }
        backBufferIdx = pSwapchain3->GetCurrentBackBufferIndex();
        signaledFenceValue++;
    }

fail:
    uint64_t messageCount = pdebugInfoQueue->GetNumStoredMessages();
    for (uint64_t idx = 0U; idx < messageCount; idx++) {
        SIZE_T message_size = 0;
        pdebugInfoQueue->GetMessage(idx, nullptr, &message_size); //get the size of the message

        D3D12_MESSAGE* message = (D3D12_MESSAGE*) malloc(message_size); //allocate enough space
        ret = pdebugInfoQueue->GetMessage(idx, message, &message_size);
        if (ret != S_OK) {
            std::cerr << "Failed to get any message\n";
        } else {
            std::cerr << "D3D12 Debug: " << message->DescriptionByteLength << " " << message->pDescription << "\n";
        }
        free(message);
    }
    pdebugInfoQueue->ClearStoredMessages();
}