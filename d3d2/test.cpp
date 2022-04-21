#include <iostream>

#include <ddraw.h>
#include <d3d.h>

static uint32_t idx;

GUID *adapterGUID = nullptr;

bool DriverEnumCallback(GUID* pGUID, TCHAR* strDesc, TCHAR* strName, VOID*, HMONITOR)
{
    std::cout << "Index : " << idx++ << "\n";
    std::cout << "strDesc: " << strDesc << " strName: " << strName << "\n";
    adapterGUID = pGUID;
    return true;
}

int main() {
    LPDIRECTDRAW lpDD;
    LPDIRECT3D   lpD3D;

    auto ret = DirectDrawCreate(nullptr, &lpDD, nullptr);
    if (ret != S_OK) {
        std::cerr << "Failed to create Ddraw\n";
        return -1;
    } else {
        std::cout << "Created ddraw\n";
    }

    ret = lpDD->QueryInterface(IID_IDirect3D, (LPVOID * )&lpD3D);
    if (ret != S_OK) {
        std::cerr << "Failed to query d3d\n";
        return -1;
    } else {
        std::cout << "Created d3d\n";
    }
}
