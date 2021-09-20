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

    if (!gc.InitializeCommandQueueResources()) {
        return -1;
    }

    if (!eventLoop.InitializeEventLoop()) {
        std::cerr << "Failed to initialize event loop\n";
        return -1;
    }   

    ///
    /// RTV Descriptor heap creation
    ///
    /// The RTV Descriptor heap creation involves the following steps:
    ///
    /// 1. Create the backing allocation for the descriptor heap. One descriptor per swapchain buffer.
    ///    This is done with a call to ID3D12Device::CreateDescriptorHeap.
    /// 2. Create Render target views for each swapchain buffer. ICDs will add VA pointers to the actual
    ///    swapchain buffer resident in vidmem in the descriptor.
    /// 
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeap = {};
    rtvDescHeap.NumDescriptors = 2; // We create one descriptor for each swapchain buffer.
    rtvDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ID3D12DescriptorHeap *pRtvDescHeap;
    /// Note: RTV heaps are always allocated in sysmem. This is fine since the actual swapchain
    ///       buffers would have already been allocated in vidmem during swapchain creation.
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
            /// Fill in the descriptor with VA pointers to the swapchain buffer.
            gc.pD3d12Device->CreateRenderTargetView(pswapchainBuffers[idx].Get(), nullptr, rtvDescHandle);
            rtvDescHandle.ptr += rtvDescIncrementSize;
        }
    }
    if (ret != S_OK) {
        std::cerr << "Failed to create RTVs for swapchain buffers\n";
        return -1;
    }

    ///
    /// Create a ID3D12Fence that can be signalled by the GPU once all required work and presents are completed.
    ///
    ComPtr<ID3D12Fence> pFence;
    uint32_t initialFenceValue = 0U;
    uint32_t signaledFenceValue = initialFenceValue + 1U;
    ret = gc.pD3d12Device->CreateFence(initialFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
    if (ret != S_OK) {
        std::cerr << "Failed to create D3d12 fence\n";
        return -1;
    }

    ///
    /// Create an unsignaled event. This event is signalled once pFence is signalled by the GPU.
    ///
    HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fenceEvent == nullptr) {
        std::cerr << "Failed to create event\n";
        return -1;
    }

    float colors[4];
    colors[3] = 0.0F;

    uint32_t backBufferIdx = gc.pSwapchain3->GetCurrentBackBufferIndex();

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

        ///
        /// Transition the RTV from Present to Render Target state.
        ///
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

        ///
        /// Insert the pre transition barrier for the RTV.
        /// This sets the RTV state to Render Target.
        ///
        gc.pCmdList->ResourceBarrier(1, &rtvPreBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE currentRtvHandle = pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        currentRtvHandle.ptr += (backBufferIdx * rtvDescIncrementSize);

        gc.pCmdList->ClearRenderTargetView(currentRtvHandle, colors, 0, nullptr);

        ///
        /// Transition the RTV from Render Target to Present state.
        ///
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
        ///
        /// NOTE: Most ICDs append a recorded command list with a fence signal method.
        ///       This fence signal method signals the completion of the execution of all GPU methods
        ///       preceeding it. The IDXGISwapChain::Present calls implicitly wait on this signal
        ///       event before proceeding with the actual flip. Hence applications need not explicitly add
        ///       a wait before a present call.
        ret = gc.pSwapchain3->Present(0, 0);
        if (ret != S_OK) {
            std::cerr << "Failed to present \n";
            return -1;
        }

        ///
        /// Add a signal method to the command queue. The fence is signalled by the GPU
        /// only after all the previously queued commands have completed execution.
        ///
        /// By this point, the command queue would look something like below:
        ///
        /// 1. RTV Pre transition barrier.
        /// 2. ClearColor method
        /// 3. RTV Post transition barrier
        /// 4. Signal method inserted by the GPU ICD
        /// 5. DXGI Present. (This isn't a method submitted to the GPU. The GPU's DC which is an entirely separate block takes care of the flip.)
        /// 6. Signal method inserted by the app below.
        ///
        ret = gc.pCommandQueue->Signal(pFence.Get(), signaledFenceValue);
        if (ret != S_OK) {
            std::cerr << "Failed to signal fence\n";
            return -1;
        }

        if (pFence->GetCompletedValue() < signaledFenceValue) {
            ///
            /// If the fence hasn't been signalled by the GPU yet (which will most probably be the case unless
            /// the submitted commands are executed faster than the time it took for the CPU to reach this point)
            /// attach an event to the fence. When the fence is signalled, the event is fired.
            ///
            if (pFence->SetEventOnCompletion(signaledFenceValue, fenceEvent) != S_OK) {
                std::cerr << "failed to set event on completion\n";
                return -1;
            }
            ///
            /// Perform a CPU-wait on the event till it is fired by the GPU.
            /// Note that we do end up stalling on the CPU here while the CPU could be used for
            /// other useful work such as preparing the commands for the next frame etc. This isn't ideal.
            ///
            WaitForSingleObject(fenceEvent, INFINITE);
        }
        backBufferIdx = gc.pSwapchain3->GetCurrentBackBufferIndex();
        ///
        /// Increment the fence value that should be waited for on the next frame.
        ///
        signaledFenceValue++;
    }
}
