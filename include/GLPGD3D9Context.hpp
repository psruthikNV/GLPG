#pragma once

#include <cstdint>
#include <d3d9.h>
#include <wrl/client.h>

namespace GLPG {
    struct GLPGD3D9Context {
        GLPGD3D9Context();
        //~GLPGD3D9Context();
        bool InitializeContext();
        bool CreateContext(HWND &hWnd);
        bool inited = false;
        uint32_t defaultWidth = 1366U;
        uint32_t defaultHeight = 768U;
        // D3D9 Resources
        Microsoft::WRL::ComPtr<IDirect3D9> pd3d9Interface; 
        //Microsoft::WRL::ComPtr<IDirect3DDevice9> pd3d9Device;
        IDirect3DDevice9 *pd3d9Device = nullptr;
      private:
        uint32_t numAdapters = 0U;
        uint32_t adapterIdxToUse = 0U;
        uint32_t numOutputs = 0U;
        uint32_t outputIdxToUse = 0U;
        uint32_t modeIdxToUse = 0U;
        D3DFORMAT displayBufferFormat = D3DFMT_X8R8G8B8;
        bool EnumerateAdapters();
    };
} // namespace GLPG
