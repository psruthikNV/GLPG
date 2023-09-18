#include "GLPGD3D11Context.hpp"
#include "GLPGWindow.hpp"
#include "internal/GLPGWindowImpl_Win32.hpp"
#include "utils/GLPGD3DUtils.hpp"

namespace GLPG {

#ifdef GLPG_IS_DEBUG
bool GLPGD3D11Context::InitializeDebugInfoQueue() {
    if (pD3d11Device->QueryInterface(IID_PPV_ARGS(&pdebugInfoQueue)) != S_OK) {
        std::cerr << "Failed to query Info Queue\n";
        return false;
    }

    if (pdebugInfoQueue->PushEmptyStorageFilter() != S_OK) {
        return false;
    }

    D3D11_INFO_QUEUE_FILTER_DESC infoQueueStorageFilterAllowDesc = {};
    D3D11_MESSAGE_SEVERITY allowedSeverity[] = {D3D11_MESSAGE_SEVERITY_CORRUPTION,
                                                D3D11_MESSAGE_SEVERITY_ERROR,
                                                D3D11_MESSAGE_SEVERITY_WARNING,
                                                D3D11_MESSAGE_SEVERITY_MESSAGE};
    infoQueueStorageFilterAllowDesc.NumSeverities = array_size(allowedSeverity);
    infoQueueStorageFilterAllowDesc.pSeverityList = allowedSeverity;
    D3D11_INFO_QUEUE_FILTER infoQueueStorageFilter = {};
    infoQueueStorageFilter.AllowList = infoQueueStorageFilterAllowDesc;

    if (pdebugInfoQueue->AddStorageFilterEntries(&infoQueueStorageFilter) != S_OK) {
        std::cerr << "Failed to add storage filter\n";
        return false;
    }

    return true;
}

void GLPGD3D11Context::InfoQueuePoller() {
    while (running != false) {
        uint64_t messageCount = pdebugInfoQueue->GetNumStoredMessages();
        for (uint64_t idx = 0U; idx < messageCount; idx++) {
            SIZE_T message_size = 0;
            pdebugInfoQueue->GetMessage(idx, nullptr, &message_size);
            D3D11_MESSAGE* message = (D3D11_MESSAGE*) malloc(message_size);
            if (pdebugInfoQueue->GetMessage(idx, message, &message_size) == S_OK) {
                std::cerr << "D3D11 Debug: " << message->pDescription << "\n";
            }
            free(message);
        }
        pdebugInfoQueue->ClearStoredMessages();
    }
}
#endif

bool GLPGD3D11Context::EnumerateAdapters() {
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

bool GLPGD3D11Context::InitializeSwapchain() {
    adapters[adapterIdxToUse]->GetParent(IID_PPV_ARGS(&pdxgiFactory2));
    if (!pdxgiFactory2) {
        std::cout << "Failed to get pdxgifactory 2\n";
        return false;
    }

    DXGI_SAMPLE_DESC sampleDesc = {};
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
    swapchainDesc.BufferCount = 2;
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY;
    swapchainDesc.SampleDesc = sampleDesc;
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchainDesc.Stereo = FALSE;
    swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    GLPGWindowImpl_Win32* windowImpl = dynamic_cast<GLPGWindowImpl_Win32*>(GLPG::GLPGWindow::GetImplInstance());
    if (!windowImpl) {
        std::cerr << "GLPG Error: Internal Failure" << std::endl;
        return false;
    }

    if (HRESULT ret = pdxgiFactory2->CreateSwapChainForHwnd(pD3d11Device.Get(), windowImpl->GetWin32WindowHandle(),
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

GLPGD3D11Context::GLPGD3D11Context() {
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

    uint32_t createDeviceFlags = 0U;
#ifdef GLPG_IS_DEBUG
    createDeviceFlags = createDeviceFlags | D3D11_CREATE_DEVICE_DEBUG;
#endif
    if (D3D11CreateDevice(adapters[adapterIdxToUse], D3D_DRIVER_TYPE_UNKNOWN,
                          NULL, createDeviceFlags, NULL, NULL,
                          D3D11_SDK_VERSION, &pD3d11Device, NULL, &pD3d11Context) != S_OK) {
        std::cerr << "Failed to create d3d11 device\n";
        inited = false;
    }

#ifdef GLPG_IS_DEBUG
    if (!InitializeDebugInfoQueue()) {
        std::cerr << "Failed to initialize debug queue\n";
        debugInited = false;
        // We'll still go ahead with the init
    } else {
        std::cerr << "Starting Info poller\n";
        InfoQueuePollThread = std::thread(&GLPGD3D11Context::InfoQueuePoller, this);
    }
#endif

}

GLPGD3D11Context::~GLPGD3D11Context() {
    running = false;
#ifdef GLPG_IS_DEBUG
    if (debugInited) {
        InfoQueuePollThread.join();
    }
#endif
}

} // namespace GLPG
