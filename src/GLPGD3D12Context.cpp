#include "GLPGD3D12Context.hpp"
#include "GLPGWindow.hpp"
#include "internal/GLPGWindowImpl_Win32.hpp"
#include "utils/GLPGD3DUtils.hpp"

namespace GLPG {

#ifdef GLPG_IS_DEBUG
bool GLPGD3D12Context::InitializeDebugInfoQueue() {
    if (pD3d12Device->QueryInterface(IID_PPV_ARGS(&pdebugInfoQueue)) != S_OK) {
        std::cerr << "Failed to query Info Queue\n";
        return false;
    }

    if (pdebugInfoQueue->PushEmptyStorageFilter() != S_OK) {
        return false;
    }

    D3D12_INFO_QUEUE_FILTER_DESC infoQueueStorageFilterAllowDesc = {};
    D3D12_MESSAGE_SEVERITY allowedSeverity[] = {D3D12_MESSAGE_SEVERITY_CORRUPTION,
                                                D3D12_MESSAGE_SEVERITY_ERROR,
                                                D3D12_MESSAGE_SEVERITY_WARNING,
                                                D3D12_MESSAGE_SEVERITY_MESSAGE};
    infoQueueStorageFilterAllowDesc.NumSeverities = array_size(allowedSeverity);
    infoQueueStorageFilterAllowDesc.pSeverityList = allowedSeverity;
    D3D12_INFO_QUEUE_FILTER infoQueueStorageFilter = {};
    infoQueueStorageFilter.AllowList = infoQueueStorageFilterAllowDesc;

    if (pdebugInfoQueue->AddStorageFilterEntries(&infoQueueStorageFilter) != S_OK) {
        std::cerr << "Failed to add storage filter\n";
        return false;
    }

    return true;
}

void GLPGD3D12Context::InfoQueuePoller() {
    while (running != false) {
        uint64_t messageCount = pdebugInfoQueue->GetNumStoredMessages();
        for (uint64_t idx = 0U; idx < messageCount; idx++) {
            SIZE_T message_size = 0;
            pdebugInfoQueue->GetMessage(idx, nullptr, &message_size); //get the size of the message

            D3D12_MESSAGE* message = (D3D12_MESSAGE*) malloc(message_size); //allocate enough space
            if (pdebugInfoQueue->GetMessage(idx, message, &message_size) == S_OK) {
                std::cerr << "D3D12 Debug: " << message->pDescription << "\n";
            }
            free(message);
        }
        pdebugInfoQueue->ClearStoredMessages();
    }
}
#endif

bool GLPGD3D12Context::InitializeCommandQueueResources(ID3D12PipelineState *pipelineState) {
    if (pD3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                             IID_PPV_ARGS(&pCmdAllocator)) != S_OK) {
        std::cerr << "Failed to create command allocator\n";
        return false;
    }

    if (pD3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                        pCmdAllocator.Get(), pipelineState, IID_PPV_ARGS(&pCmdList)) != S_OK) {
        std::cerr << "Failed to create command list\n";
        return false;
    }

    if (pCmdList->Close() != S_OK) {
        std::cerr << "Failed to close command list\n";
        return false;
    }

    return true;
}

bool GLPGD3D12Context::InitializeSwapchain() {
    adapters[adapterIdxToUse]->GetParent(IID_PPV_ARGS(&pdxgiFactory2));
    if (!pdxgiFactory2) {
        std::cout << "Failed to get pdxgifactory 2\n";
        return false;
    }

    DXGI_SAMPLE_DESC sampleDesc = { 0 };
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    DXGI_SWAP_CHAIN_DESC1 swapchainDesc = { 0 };
    // BufferCount must be > 1 for DXGI_SWAP_EFFECT_FLIP_DISCARD.
    swapchainDesc.BufferCount = 2;
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY;
    swapchainDesc.SampleDesc = sampleDesc;
    // D3D12 does not support blitting swap effects and only flip modes must be used.
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchainDesc.Stereo = FALSE;
    swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    GLPGWindowImpl_Win32* windowImpl = dynamic_cast<GLPGWindowImpl_Win32*>(GLPG::GLPGWindow::GetImplInstance());
    if (!windowImpl) {
        std::cerr << "GLPG Error: Internal Failure" << std::endl;
        return false;
    }

    if (HRESULT ret = pdxgiFactory2->CreateSwapChainForHwnd(pCommandQueue.Get(), windowImpl->GetWin32WindowHandle(),
                                              &swapchainDesc, NULL, NULL, &pSwapchain); ret != S_OK) {
        std::cout << "Swapchain creation failed due to : " << std::hex << ret << std::dec << "\n";
        return false;
    }

    if (pSwapchain->QueryInterface(IID_PPV_ARGS(&pSwapchain3)) != S_OK) {
        std::cerr << "Failed to query IDXGISwapChain3\n";
        return false;
    }

    return true;
}

bool GLPGD3D12Context::EnumerateAdapters() {
    IDXGIAdapter* pAdapter = nullptr;
    while (pdxgiFactory->EnumAdapters(numAdapters, &pAdapter) !=
           DXGI_ERROR_NOT_FOUND) {
        adapters.push_back(pAdapter);
        ++numAdapters;
    }
    std::cout << "Number of adapters: " << numAdapters << "\n\n";
    if (numAdapters == 0U) {
        return false;
    }

    DXGI_ADAPTER_DESC pdesc = {};

    // By default we select the GPU with the most vidmem
    // as the adapter to use.
    // TODO: Check for the number of outputs connected to the adapter here.
    for (uint32_t idx = 0U; idx < numAdapters; idx++) {
        auto ret = adapters[idx]->GetDesc(&pdesc);
        std::cout << "Details of Adapter " << idx + 1 << "\n";
        std::wcout << "Description : " << pdesc.Description << "\n";
        std::cout << "Vidmem : " << pdesc.DedicatedVideoMemory / 1000000000 << "GB\n";
        std::cout << "Vidmem : " << pdesc.DedicatedVideoMemory / 1000000 << "MB\n";
        std::cout << "Dedicated Sysmem : "
            << pdesc.DedicatedSystemMemory / 1000000000 << "GB\n";
        std::cout << "Shared Sysmem : " << pdesc.SharedSystemMemory / 1000000000
            << "GB\n\n";
        if (pdesc.DedicatedVideoMemory > 0 &&
            pdesc.DedicatedVideoMemory > adapterVidMem) {
            adapterIdxToUse = idx;
            adapterVidMem = pdesc.DedicatedSystemMemory;
        }
    }

    return true;
}

GLPGD3D12Context::GLPGD3D12Context() {
#ifdef GLPG_IS_DEBUG
    if (D3D12GetDebugInterface(IID_PPV_ARGS(&pdebugInterface)) != S_OK) {
        // Even though enabling the debug layer failed, we can go ahead
        // with the init. Just log the error.
        std::cerr << "Failed to get d3d12 debug interface\n";
    } else {
        pdebugInterface->EnableDebugLayer();
    }
#endif
    if (CreateDXGIFactory(IID_PPV_ARGS(&pdxgiFactory)) != S_OK) {
        std::cerr << "CreateDXGIFactory failed\n";
        inited = false;
        return;
    }

    if (!EnumerateAdapters()) {
        std::cerr << "Failed to enumerate adapters\n";
        inited = false;
        return;
    }

    if (D3D12CreateDevice(adapters[adapterIdxToUse], D3D_FEATURE_LEVEL_12_1,
                          IID_PPV_ARGS(&pD3d12Device)) != S_OK) {
        std::cerr << "Failed to create d3d12 device\n";
        inited = false;
        return;
    }
#ifdef GLPG_IS_DEBUG
    if (!InitializeDebugInfoQueue()) {
        std::cerr << "Failed to initialize debug queue\n";
        debugInited = false;
        // We'll still go ahead with the init
    } else {
        std::cerr << "Starting Info poller\n";
        InfoQueuePollThread = std::thread(&GLPGD3D12Context::InfoQueuePoller, this);
    }
#endif

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.NodeMask = 0U;

    if (pD3d12Device->CreateCommandQueue(&commandQueueDesc, 
                                         IID_PPV_ARGS(&pCommandQueue)) != S_OK) {
        std::cerr << "Failed to create command queue\n";
        inited = false;
        return;
    }
}

GLPGD3D12Context::~GLPGD3D12Context() {
    running = false;
#ifdef GLPG_IS_DEBUG
    if (debugInited) {
        InfoQueuePollThread.join();
    }
#endif
}

} // namespace GLPG
