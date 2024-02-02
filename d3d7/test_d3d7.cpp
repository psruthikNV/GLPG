#include <iostream>

#define INITGUID
#include <ddraw.h>
#include <d3d.h>

static uint32_t idx;

GUID* adapterGUID;
bool DriverEnumCallback(GUID* pGUID, TCHAR* strDesc, TCHAR* strName, VOID*, HMONITOR)
{
    std::cout << "Index : " << idx++ << "\n";
    std::cout << "strDesc: " << strDesc << " strName: " << strName << "\n";
    std::cout << "GUID: " << pGUID << "\n";
    adapterGUID = pGUID;
    return true;
}

int main() {
    auto hr = DirectDrawEnumerateEx(reinterpret_cast<LPDDENUMCALLBACKEX>(DriverEnumCallback), nullptr,
                                    DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES);
    if (hr == DD_OK) {
        std::cerr << "Enumerated Devices\n";
    } else {
        std::cerr << "Failed to enumerate devices\n";
        std::cerr << "Err: " << std::hex << hr << "\n";
        return -1;
    }

    LPDIRECTDRAW7 pDD;
    IDirectDraw7* ddraw = nullptr;
    auto ret = DirectDrawCreateEx(nullptr, (void**)&pDD, IID_IDirectDraw7, nullptr);
    if (ret == S_OK) {
        std::cerr << "Created Direct Draw\n";
    } else {
        std::cerr << "Failed to create DDraw\n";
        std::cerr << "Err: " << std::hex << ret << "\n";
        return -1;
    }


    LPDIRECT3D7 pD3D;
    ret = pDD->QueryInterface(IID_IDirect3D7, (void**)&pD3D);
    if (ret != S_OK) {
        std::cerr << "Failed to query D3D7 from DD\n";
        std::cerr << "Err: " << std::hex << ret << "\n";
        return -1;
    }

    DDCAPS ddCaps;
    ddCaps.dwSize = sizeof(DDCAPS);
    ret = pDD->GetCaps(&ddCaps, nullptr);
    if (ret == S_OK) {
        std::cout << "DD Caps: \n";
        if (ddCaps.dwCaps & DDCAPS_3D) {
            std::cout << "DDCAPS_3D\n";
        }
        if (ddCaps.dwCaps & DDCAPS_ALIGNBOUNDARYDEST) {
            std::cout << "DDCAPS_ALIGNBOUNDARYDEST\n";
        }
        if (ddCaps.dwCaps & DDCAPS_ALIGNSIZEDEST) {
            std::cout << "DDCAPS_ALIGNSIZEDEST\n";
        }
        if (ddCaps.dwCaps & DDCAPS_ALIGNBOUNDARYSRC) {
            std::cout << "DDCAPS_ALIGNBOUNDARYSRC\n";
        }
        if (ddCaps.dwCaps & DDCAPS_ALIGNSIZESRC) {
            std::cout << "DDCAPS_ALIGNSIZESRC\n";
        }
        if (ddCaps.dwCaps & DDCAPS_ALIGNSTRIDE) {
            std::cout << "DDCAPS_ALIGNSTRIDE\n";
        }
        if (ddCaps.dwCaps & DDCAPS_BANKSWITCHED) {
            std::cout << "DDCAPS_BANKSWITCHED\n";
        }
        if (ddCaps.dwCaps & DDCAPS_BLT) {
            std::cout << "DDCAPS_BLT\n";
        }
        if (ddCaps.dwCaps & DDCAPS_BLTCOLORFILL) {
            std::cout << "DDCAPS_BLTCOLORFILL\n";
        }
        if (ddCaps.dwCaps & DDCAPS_BLTQUEUE) {
            std::cout << "DDCAPS_BLTQUEUE\n";
        }
        if (ddCaps.dwCaps & DDCAPS_BLTFOURCC) {
            std::cout << "DDCAPS_BLTFOURCC\n";
        }
        if (ddCaps.dwCaps & DDCAPS_BLTSTRETCH) {
            std::cout << "DDCAPS_BLTSTRETCH\n";
        }
        if (ddCaps.dwCaps & DDCAPS_GDI) {
            std::cout << "DDCAPS_GDI\n";
        }
        if (ddCaps.dwCaps & DDCAPS_OVERLAY) {
            std::cout << "DDCAPS_OVERLAY\n";
        }
        if (ddCaps.dwCaps & DDCAPS_OVERLAYCANTCLIP) {
            std::cout << "DDCAPS_OVERLAYCANTCLIP\n";
        }
        if (ddCaps.dwCaps & DDCAPS_OVERLAYFOURCC) {
            std::cout << "DDCAPS_OVERLAYFOURCC\n";
        }
        if (ddCaps.dwCaps & DDCAPS_OVERLAYSTRETCH) {
            std::cout << "DDCAPS_OVERLAYSTRETCH\n";
        }
        if (ddCaps.dwCaps & DDCAPS_PALETTE) {
            std::cout << "DDCAPS_PALETTE\n";
        }
        if (ddCaps.dwCaps & DDCAPS_PALETTEVSYNC) {
            // MSDN docs refer to this as DDCAPS_PALETTECANVSYNC
            // which isn't defined in the current version of ddraw.h
            std::cout << "DDCAPS_PALETTEVSYNC\n";
        }
        if (ddCaps.dwCaps & DDCAPS_READSCANLINE) {
            std::cout << "DDCAPS_READSCANLINE\n";
        }
        /* DDCAPS_STEREOVIEW is not defined anymore.
        if (ddCaps.dwCaps & DDCAPS_STEREOVIEW) {
            std::cout << "DDCAPS_STEREOVIEW\n";
        }*/
        if (ddCaps.dwCaps & DDCAPS_VBI) {
            std::cout << "DDCAPS_VBI\n";
        }
        if (ddCaps.dwCaps & DDCAPS_ZBLTS) {
            std::cout << "DDCAPS_ZBLTS\n";
        }
        if (ddCaps.dwCaps & DDCAPS_ZOVERLAYS) {
            std::cout << "DDCAPS_ZOVERLAYS\n";
        }
        if (ddCaps.dwCaps & DDCAPS_COLORKEY) {
            std::cout << "DDCAPS_COLORKEY\n";
        }
        if (ddCaps.dwCaps & DDCAPS_ALPHA) {
            std::cout << "DDCAPS_ALPHA\n";
        }
        if (ddCaps.dwCaps & DDCAPS_COLORKEYHWASSIST) {
            // MSDN docs refer to this as DDCAPS_COLORKEY_HWASSIST
            // which isn't defined in the current version of ddraw.h
            std::cout << "DDCAPS_COLORKEYHWASSIST\n";
        }
        if (ddCaps.dwCaps & DDCAPS_NOHARDWARE) {
            std::cout << "DDCAPS_NOHARDWARE\n";
        }
    } else {
        std::cerr << "Failed to retrieve DD caps\n";
        std::cerr << "Err: " << std::hex << ret << "\n";
        return -1;
    }

    DDSCAPS2      ddsCaps2 { };
    DWORD         dwTotal;
    DWORD         dwFree;
    ret = pDD->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree);
    if (ret == S_OK) {
        std::cout << "Vidmem: " << dwTotal << " Free Vidmem: " << dwFree << "\n";
    }

    DDDEVICEIDENTIFIER2 deviceIdentifier { };
    ret = pDD->GetDeviceIdentifier(&deviceIdentifier, 0);
    if (ret == S_OK) {
        std::cout << "Driver: " << deviceIdentifier.szDriver << " Description: " << deviceIdentifier.szDescription << "\n";
    } else {
        std::cout << "Failed to query device identifier\n";
        std::cerr << "Err: " << std::hex << ret << "\n";
    }
}
