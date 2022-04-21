#include "GLPGD3D9Context.hpp"
#include "GLPGWindow.hpp"
#include "internal/GLPGWindowImpl_Win32.hpp"
#include "utils/GLPGD3DUtils.hpp"

namespace GLPG {
    GLPGD3D9Context::GLPGD3D9Context() {
        pd3d9Interface = Direct3DCreate9(D3D_SDK_VERSION);
        if (pd3d9Interface == nullptr) {
            std::cerr << "Failed to create D3D9 interface\n";
            return;
        }
        if (!EnumerateAdapters()) {
            std::cerr << "Failed to enumerate adapters/outputs\n";
            return;
        }
        inited = true;
    }

    bool GLPGD3D9Context::EnumerateAdapters() {
        numAdapters = pd3d9Interface->GetAdapterCount();

        for (uint32_t idx = 0U; idx < numAdapters; idx++) {
            D3DADAPTER_IDENTIFIER9 pAdapterIdentifier = {};
            if (pd3d9Interface->GetAdapterIdentifier(idx, 0, &pAdapterIdentifier) == S_OK) {
                std::cout << "Adapter " << idx << "\n";
                std::cout << "Driver: " << pAdapterIdentifier.Driver << "\n";
                std::cout << "Description: " << pAdapterIdentifier.Description << "\n";
                std::cout << "DeviceName: " << pAdapterIdentifier.DeviceName << "\n";
                std::cout << "VendorId: " << pAdapterIdentifier.VendorId << "\n";
            }
        }

        // FIXME: D3D9 doesn't allow querying of vidmem before device creation. Default to using the first
        //        enumerated adapter for now.
        adapterIdxToUse = 0U;

        uint32_t numModes = pd3d9Interface->GetAdapterModeCount(adapterIdxToUse, displayBufferFormat);
        uint32_t largestWidth = 640U;
        uint32_t minRefreshRate = 60U;
        for (uint32_t idx = 0U; idx < numModes; idx++) {
            D3DDISPLAYMODE mode = {};
            if (pd3d9Interface->EnumAdapterModes(adapterIdxToUse, displayBufferFormat, idx, &mode) == S_OK) {
                //std::cout << "Mode " << idx << " : " << mode.Width << "x" << mode.Height << "@" << mode.RefreshRate << "\n\n";
                if (mode.Width >= largestWidth) {
                    if (mode.RefreshRate >= minRefreshRate) {
                        largestWidth = mode.Width;
                        minRefreshRate = mode.RefreshRate;
                        modeIdxToUse = idx;
                        defaultWidth = mode.Width;
                        defaultHeight = mode.Height;
                    }
                }
            } else {
                return false;
            }
        }
        return true;
    }

    bool GLPGD3D9Context::CreateContext(HWND &hWnd) {
        D3DPRESENT_PARAMETERS  presentParameters = {};
        presentParameters.BackBufferWidth = defaultWidth;
        presentParameters.BackBufferHeight = defaultHeight;
        presentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
        presentParameters.BackBufferCount = 1U;
        presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
        presentParameters.MultiSampleQuality = 0U;
        presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        presentParameters.hDeviceWindow = hWnd;
        presentParameters.Windowed = false;
        presentParameters.EnableAutoDepthStencil = false;
        presentParameters.FullScreen_RefreshRateInHz = 144U;
        presentParameters.PresentationInterval = 1U;

        HRESULT ret;
        if ((ret = pd3d9Interface->CreateDevice(adapterIdxToUse, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters,
                                         &pd3d9Device)) != S_OK) {
            std::cerr << "Failed to create D3D9 device. ret = " << ret << " \n";
            return false;
        } else {
            return true;
        }
    }
} // namespace GLPG
