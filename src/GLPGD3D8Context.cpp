#include "GLPGD3D8Context.hpp"
#include "GLPGWindow.hpp"
#include "internal/GLPGWindowImpl_Win32.hpp"
#include "utils/GLPGD3DUtils.hpp"

namespace GLPG {
    GLPGD3D8Context::GLPGD3D8Context() {
        pd3d8Interface = Direct3DCreate8(D3D_SDK_VERSION);
        if (pd3d8Interface == nullptr) {
            std::cerr << "Failed to create D3D9 interface\n";
            return;
        }
        if (!EnumerateAdapters()) {
            std::cerr << "Failed to enumerate adapters/outputs\n";
            return;
        }
        inited = true;
    }

    bool GLPGD3D8Context::EnumerateAdapters() {
        numAdapters = pd3d8Interface->GetAdapterCount();

        for (uint32_t idx = 0U; idx < numAdapters; idx++) {
            D3DADAPTER_IDENTIFIER8 pAdapterIdentifier = {};
            if (pd3d8Interface->GetAdapterIdentifier(idx, 0, &pAdapterIdentifier) == S_OK) {
                std::cout << "Adapter " << idx << "\n";
                std::cout << "Driver: " << pAdapterIdentifier.Driver << "\n";
                std::cout << "Description: " << pAdapterIdentifier.Description << "\n";
                std::cout << "DeviceId: " << pAdapterIdentifier.DeviceId << "\n";
                std::cout << "VendorId: " << pAdapterIdentifier.VendorId << "\n";
            }
        }

        // FIXME: D3D8 doesn't allow querying of vidmem before device creation. Default to using the first
        //        enumerated adapter for now.
        adapterIdxToUse = 0U;

        uint32_t numModes = pd3d8Interface->GetAdapterModeCount(adapterIdxToUse);
        uint32_t largestWidth = 640U;
        uint32_t minRefreshRate = 60U;
        for (uint32_t idx = 0U; idx < numModes; idx++) {
            D3DDISPLAYMODE mode = {};
            if (pd3d8Interface->EnumAdapterModes(adapterIdxToUse, idx, &mode) == S_OK) {
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

    bool GLPGD3D8Context::CreateContext(HWND &hWnd) {
        D3DPRESENT_PARAMETERS  presentParameters = {};
        presentParameters.BackBufferWidth = defaultWidth;
        presentParameters.BackBufferHeight = defaultHeight;
        presentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
        presentParameters.BackBufferCount = 1U;
        presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
        presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        presentParameters.hDeviceWindow = hWnd;
        presentParameters.Windowed = false;
        presentParameters.EnableAutoDepthStencil = false;
        presentParameters.FullScreen_RefreshRateInHz = 144U;
        presentParameters.FullScreen_PresentationInterval = 1U;

        HRESULT ret;
        if ((ret = pd3d8Interface->CreateDevice(adapterIdxToUse, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters,
                                         &pd3d8Device)) != S_OK) {
            std::cerr << "Failed to create D3D8 device. ret = " << ret << " \n";
            return false;
        } else {
            return true;
        }
    }
} // namespace GLPG
