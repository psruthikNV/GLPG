#pragma once

#include <thread>
#include <vector>

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

namespace GLPG {
    struct GLPGD3D12Context {
            GLPGD3D12Context();
            ~GLPGD3D12Context();

#ifdef GLPG_IS_DEBUG
            bool InitializeDebugInfoQueue();
            void InfoQueuePoller();
#endif
            bool InitializeContext();
            bool SwapBuffers();
            bool InitializeSwapchain();

            bool inited = true;
            uint32_t defaultWidth = 1366U;
            uint32_t defaultHeight = 768U;
            // D3d12 resources
            Microsoft::WRL::ComPtr<IDXGIFactory> pdxgiFactory;
            Microsoft::WRL::ComPtr<ID3D12Device> pD3d12Device;
            Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQueue;
            Microsoft::WRL::ComPtr<IDXGIFactory2> pdxgiFactory2;
            Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapchain;
            Microsoft::WRL::ComPtr<IDXGISwapChain3> pSwapchain3;
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pCmdAllocator;
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> pCmdList;
            std::vector<IDXGIAdapter*> adapters;
            std::vector<IDXGIOutput*> outputs;
#ifdef GLPG_IS_DEBUG
            Microsoft::WRL::ComPtr<ID3D12Debug> pdebugInterface;
            Microsoft::WRL::ComPtr<ID3D12InfoQueue> pdebugInfoQueue;
            std::thread InfoQueuePollThread;
#endif
        private:
            uint32_t numAdapters = 0U;
            uint32_t adapterIdxToUse = 0U;
            uint32_t adapterVidMem = 0U;
            uint32_t numOutputs = 0U;
            uint32_t outputIdxToUse = 0U;
            bool EnumerateAdapters();
            bool EnumerateOutputs();
            bool InitializeCommandQueueResources();
            bool running = true; // The destructor sets this to false
#ifdef GLPG_IS_DEBUG
            bool debugInited = true;
#endif
    };
} // namespace GLPG