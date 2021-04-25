#include <iostream>
#include <vector>
#include <random>
#include <fstream>

#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"
#include "../include/GLPGD3D12Context.hpp"

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

    GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    if (!window) {
        std::cerr << "Failed to create GLPGWindow\n";
        return -1;
    }

    GLPG::GLPGD3D12Context gc;
    if (!gc.inited) {
        std::cerr << "Failed to initialize D3D12\n";
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

    uint32_t backBufferIdx = gc.pSwapchain3->GetCurrentBackBufferIndex();

    D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeap = {};
    rtvDescHeap.NumDescriptors = 2;
    rtvDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ID3D12DescriptorHeap *pRtvDescHeap;
    ret = gc.pD3d12Device->CreateDescriptorHeap(&rtvDescHeap, IID_PPV_ARGS(&pRtvDescHeap));
    if (ret != S_OK) {
        std::cerr << "Failed to create RTV Descriptor heap\n";
    }

    D3D12_CPU_DESCRIPTOR_HANDLE rtvDescHandle = pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
    uint32_t rtvDescIncrementSize = gc.pD3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    ComPtr<ID3D12Resource> pswapchainBuffers[2];
    for (uint32_t idx = 0; idx < 2; idx++) {
        ret = gc.pSwapchain3->GetBuffer(idx, IID_PPV_ARGS(&pswapchainBuffers[idx]));
        if (ret != S_OK) {
            std::cerr << "Failed to get buffer\n";
        } else {
            gc.pD3d12Device->CreateRenderTargetView(pswapchainBuffers[idx].Get(), nullptr, rtvDescHandle);
            rtvDescHandle.ptr += rtvDescIncrementSize;
        }
    }
    if (ret != S_OK) {
        std::cerr << "Failed to create RTVs for swapchain buffers\n";
        return -1;
    }

    ComPtr<ID3D12Fence> pFence;
    uint32_t initialFenceValue = 0U;
    uint32_t signaledFenceValue = initialFenceValue + 1U;
    ret = gc.pD3d12Device->CreateFence(initialFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
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

        ret = gc.pCmdAllocator->Reset();
        if (ret != S_OK) {
            std::cerr << "Failed to reset command allocator: " << std::hex << ret << std::dec << "\n" ;
            return -1;
        }

        ret = gc.pCmdList->Reset(gc.pCmdAllocator.Get(), nullptr);
        if (ret != S_OK) {
            std::cerr << "failed to reset command list\n";
            return -1;
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

        gc.pCmdList->ResourceBarrier(1, &rtvPreBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE currentRtvHandle = pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        currentRtvHandle.ptr += (backBufferIdx * rtvDescIncrementSize);

        gc.pCmdList->ClearRenderTargetView(currentRtvHandle, colors, 0, nullptr);

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

        gc.pCmdList->ResourceBarrier(1, &rtvPostBarrier);

        if ((ret = gc.pCmdList->Close()) != S_OK) {
            std::cerr << "Failed to close command list " << std::hex << ret << std::dec << "\n" ;
            return -1;
        }

        ID3D12CommandList *ppCmdList;
        ret = gc.pCmdList->QueryInterface(IID_PPV_ARGS(&ppCmdList));
        if (ret != S_OK) {
            std::cerr << "Failed to get cmdlist\n";
            return -1;
        }
        gc.pCommandQueue->ExecuteCommandLists(1U, &ppCmdList);

        ret = gc.pSwapchain3->Present(0, 0);
        if (ret != S_OK) {
            std::cerr << "Failed to present \n";
            return -1;
        }

        ret = gc.pCommandQueue->Signal(pFence.Get(), signaledFenceValue);
        if (ret != S_OK) {
            std::cerr << "Failed to signal fence\n";
            return -1;
        }

        if (pFence->GetCompletedValue() < signaledFenceValue) {
            if (pFence->SetEventOnCompletion(signaledFenceValue, fenceEvent) != S_OK) {
                std::cerr << "failed to set event on completion\n";
                return -1;
            }
            WaitForSingleObject(fenceEvent, INFINITE);
        }
        backBufferIdx = gc.pSwapchain3->GetCurrentBackBufferIndex();
        signaledFenceValue++;
    }
}