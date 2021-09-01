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

            ///
            /// @brief Default constructor.
            ///
            /// Performs the following:
            ///
            /// 1. Queries and enables the debug interface if GLPG_IS_DEBUG is defined.
            /// 2. Creates a IDXGIFactory object by invoking CreateDXGIFactory.
            /// 3. Invokes EnumerateAdapters and EnumerateOutputs to find an adapter
            ///    and a output to use.
            /// 4. Creates the D3D12Device object by invoking D3D12CreateDevice on the active adapter.
            /// 5. Initializes the InfoQueue if GLPG_IS_DEBUG is defined.
            /// 6. Creates a normal priority command queue.
            ///
            /// @returns None.
            ///
            GLPGD3D12Context();

            ///
            /// @brief Default destructor
            ///
            /// Sets running to false and waits for
            /// InfoQueuePollThread to complete if GLPG_IS_DEBUG is defined.
            ~GLPGD3D12Context();

#ifdef GLPG_IS_DEBUG
            ///
            /// @brief Initializes a ID3D12InfoQueue and adds a storage filter that
            ///        filters messages of the following severity:
            ///        - D3D12_MESSAGE_SEVERITY_CORRUPTION
            ///        - D3D12_MESSAGE_SEVERITY_ERROR
            ///        - D3D12_MESSAGE_SEVERITY_WARNING
            ///        - D3D12_MESSAGE_SEVERITY_MESSAGE
            ///
            /// @note This function is enabled only if GLPG_IS_DEBUG is defined.
            ///
            /// @returns bool
            ///
            /// @retval false On failure
            /// @retval true On success
            ///
            bool InitializeDebugInfoQueue();

            ///
            /// @brief Function invoked by InfoQueuePollThread that retrieves
            ///        and displays on stderr the list of messages stored in pdebugInfoQueue,
            ///        The stored messages in pdebugInfoQueue are then cleared.
            ///
            /// @note This function is enabled only if GLPG_IS_DEBUG is defined.
            ///
            /// @returns None.
            ///
            void InfoQueuePoller();
#endif

            ///
            /// @brief Initializes pSwapchain by invoking IDXGIFactory2::CreateSwapChainForHwnd.
            ///        An IDXGIFactory2 object is acquired by invoking IDXGIObject::GetParent on
            ///        the active IDXGIAdapter.
            ///
            /// The following parameters are set for the creation of the swapchain:
            /// DXGI_SWAP_CHAIN_DESC1.BufferCount = 2
            /// DXGI_SWAP_CHAIN_DESC1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT
            /// DXGI_SWAP_CHAIN_DESC1.Flags = DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY
            /// DXGI_SWAP_CHAIN_DESC1.SampleDesc.Count = 1
            /// DXGI_SWAP_CHAIN_DESC1.SampleDesc.Quality = 0
            /// DXGI_SWAP_CHAIN_DESC1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD
            /// DXGI_SWAP_CHAIN_DESC1.Format = DXGI_FORMAT_R8G8B8A8_UNORM
            /// DXGI_SWAP_CHAIN_DESC1.Stereo = FALSE
            /// DXGI_SWAP_CHAIN_DESC1.AlphaMode = DXGI_ALPHA_MODE_IGNORE
            ///
            /// @returns bool
            ///
            /// @retval false On failure
            /// @retval true On success
            ///
            bool InitializeSwapchain();

            ///
            /// @brief Initializes a D3D12_COMMAND_LIST_TYPE_DIRECT ID3D12CommandAllocator and
            ///        a D3D12_COMMAND_LIST_TYPE_DIRECT ID3D12GraphicsCommandList
            ///        for the given ID3D12PipelineState. The created command list is then closed.
            ///
            /// @param pipelineState Pointer to a ID3D12PipelineState object on which
            ///                      the graphics command list will be created. If nullptr
            ///                      is specified, the created graphics command list will
            ///                      not be associated with any pipeline state. The pipeline state
            ///                      can be done at a later point with a call to
            ///                      ID3D12GraphicsCommandList::SetPipelineState.
            ///
            /// @returns bool
            ///
            /// @retval false On failure
            /// @retval true On success
            ///
            bool InitializeCommandQueueResources(ID3D12PipelineState *pipelineState = nullptr);

            bool inited = true; ///< Stores the status of execution of the GLPGD3D12Context ctor.
            uint32_t defaultWidth = 1366U; ///< Default height of the swapchain buffers.
            uint32_t defaultHeight = 768U; ///< Default width of the swapchain buffers.

            // D3d12 resources
            Microsoft::WRL::ComPtr<IDXGIFactory> pdxgiFactory; ///< ComPtr to a IDXGIFactory.
            Microsoft::WRL::ComPtr<ID3D12Device> pD3d12Device; ///< ComPtr to a ID3D12Device.
            Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQueue; ///< ComPtr to a ID3D12CommandQueue.
            Microsoft::WRL::ComPtr<IDXGIFactory2> pdxgiFactory2; ///< ComPtr to a IDXGIFactory2.
            Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapchain; ///< ComPtr to a IDXGISwapChain1.
            Microsoft::WRL::ComPtr<IDXGISwapChain3> pSwapchain3; ///< ComPtr to a IDXGISwapChain3.
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pCmdAllocator; ///< ComPtr to a ID3D12CommandAllocator.
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> pCmdList; ///< ComPtr to a ID3D12GraphicsCommandList.
            std::vector<IDXGIAdapter*> adapters; ///< Array of pointers to the list of available IDXGIAdapters.
            std::vector<IDXGIOutput*> outputs; ///< Array of pointers to the list of available IDXGIOutputs.
#ifdef GLPG_IS_DEBUG
            Microsoft::WRL::ComPtr<ID3D12Debug> pdebugInterface; ///< ComPtr to the ID3D12Debug interface.
            Microsoft::WRL::ComPtr<ID3D12InfoQueue> pdebugInfoQueue; ///< ComPtr to ID3D12InfoQueue.
            std::thread InfoQueuePollThread; ///< The debug thread for polling messages stored in pdebugInfoQueue.
#endif
        private:
            uint32_t numAdapters = 0U; ///< Number of available adapters.
            uint32_t adapterIdxToUse = 0U; ///< Index to the adapter to be used in adapters.
            uint32_t adapterVidMem = 0U; ///< Vidmem of the active adapter.
            uint32_t numOutputs = 0U; ///< Number of available outputs.
            uint32_t outputIdxToUse = 0U; ///< Index to the output to be used in outputs.
            bool running = true; ///< Specifies the state of the program. Set to false by ~GLPGD3D12Context.
#ifdef GLPG_IS_DEBUG
            bool debugInited = true; ///< Specifies if InitializeDebugInfoQueue succeded.
#endif

            ///
            /// @brief Enumerates the list of available adapters as reported by IDXGIFactory::EnumAdapters.
            ///        By default the adapter with the most vidmem as reported via DXGI_ADAPTER_DESC.DedicatedVideoMemory
            ///        is selected as the adapter to be used. adapterIdxToUse is set to the same adapter.
            ///
            /// @note This function fails on configurations where the adapter with the most vidmem
            ///       does not have an output connected. This is common in MSHybrid where the igpu has the least
            ///       vidmem compared to a dgpu in the same system but only the igpu might have outputs connected.
            ///
            /// @todo Account for the number of outputs connected to the adapter in addition to the vidmem when selecting
            ///       an adapter.
            ///
            /// @returns bool
            ///
            /// @retval false On failure.
            /// @retval true On success.
            ///
            bool EnumerateAdapters();

            ///
            /// @brief Enumerates the list of available outputs as reported by IDXGIAdapter::EnumOutputs.
            ///        By default the output with the maximum resolution
            ///        (calculated as DXGI_OUTPUT_DESC.DesktopCoordinates.right * DXGI_OUTPUT_DESC.DesktopCoordinates.bottom
            ///        is selected as the active output. outputIdxToUse is also set to the same output.
            ///
            /// @returns bool
            ///
            /// @retval false On failure.
            /// @retval true On success.
            ///
            bool EnumerateOutputs();
    };
} // namespace GLPG
