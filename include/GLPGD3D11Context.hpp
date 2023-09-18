#pragma once

#include <thread>
#include <vector>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

namespace GLPG {
    struct GLPGD3D11Context {
            GLPGD3D11Context();
            ~GLPGD3D11Context();

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
            // D3D11 resources
            Microsoft::WRL::ComPtr<IDXGIFactory> pdxgiFactory;
            Microsoft::WRL::ComPtr<IDXGIFactory2> pdxgiFactory2;
            Microsoft::WRL::ComPtr<ID3D11Device> pD3d11Device;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> pD3d11Context;
            Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapchain;
            Microsoft::WRL::ComPtr<IDXGISwapChain3> pSwapchain3;
            std::vector<IDXGIAdapter*> adapters;
#ifdef GLPG_IS_DEBUG
            Microsoft::WRL::ComPtr<ID3D11InfoQueue> pdebugInfoQueue;
            std::thread InfoQueuePollThread;
#endif
        private:
            uint32_t numAdapters = 0U;
            uint32_t adapterIdxToUse = 0U;
            uint32_t adapterVidMem = 0U;
            bool EnumerateAdapters();
            bool running = true; // The destructor sets this to false
#ifdef GLPG_IS_DEBUG
            bool debugInited = true;
#endif  
    };
} // namespace GLPG

