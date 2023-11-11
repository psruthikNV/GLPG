#pragma once

#include <cstdint>
#include <d3d8.h>
#include <wrl/client.h>

namespace GLPG {
    struct GLPGD3D8Context {
        GLPGD3D8Context();
        bool InitializeContext();
        bool CreateContext(HWND &hWnd);
        bool inited = false;
        uint32_t defaultWidth = 1366U;
        uint32_t defaultHeight = 768U;
        // D3D8 Resources
        Microsoft::WRL::ComPtr<IDirect3D8> pd3d8Interface; 
        IDirect3DDevice8 *pd3d8Device = nullptr;
      private:
        uint32_t numAdapters = 0U;
        uint32_t adapterIdxToUse = 0U;
        uint32_t numOutputs = 0U;
        uint32_t outputIdxToUse = 0U;
        uint32_t modeIdxToUse = 0U;
        bool EnumerateAdapters();
    };
} // namespace GLPG
