#include <iostream>

#include <ddraw.h>
#include <d3d.h>

#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"

static uint32_t idx;

LPGUID adapterGUID[3];
DWORD zBufferDepth = {};

std::string ToString(GUID *guid) {
    char guid_string[37]; // 32 hex chars + 4 hyphens + null terminator
    snprintf(
          guid_string, sizeof(guid_string),
          "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
          guid->Data1, guid->Data2, guid->Data3,
          guid->Data4[0], guid->Data4[1], guid->Data4[2],
          guid->Data4[3], guid->Data4[4], guid->Data4[5],
          guid->Data4[6], guid->Data4[7]);
    return guid_string;
}

// The DX SDK uses "PASCAL" as the calling convention
// but this is typedef'd to WINAPI in modern compilers. Use the same.
HRESULT WINAPI DriverEnumCallback(LPGUID lpGuid, LPSTR lpDeviceDescription,
    LPSTR lpDeviceName, LPD3DDEVICEDESC lpD3DHWDeviceDesc,
    LPD3DDEVICEDESC lpD3DHELDeviceDesc, LPVOID lpUserArg)
{
    std::cout << "Index : " << idx << "\n";
    std::cout << "lpDeviceDescription: " << lpDeviceDescription << " lpDeviceName: " << lpDeviceName << "\n";
    adapterGUID[idx++] = lpGuid; // The last adapter retrieved is the D3D HAL that we're interested in.
    std::cout << "lpGuid: " << ToString(lpGuid) << "\n";
	zBufferDepth = lpD3DHWDeviceDesc->dwDeviceZBufferBitDepth;
	std::cout << "zBufferDepth: " << zBufferDepth << "\n";

    return true;
}

static int32_t texFormatIdx;	
HRESULT WINAPI TextureFormatEnumCallback(LPDDSURFACEDESC lpDdsd, LPVOID lpUserArg)
{
	std::cout << "Tex : " << ++texFormatIdx << "\n";
	return true;
}

int main() {

    uint32_t currentWidth = 640U;
    uint32_t currentHeight = 480U; 
    GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;

    if (!window) {
        std::cerr << "Failed to create GLPGWindow\n";
        return -1;
    }

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
        std::cout << "Queried d3d\n";
    }

    ret = lpD3D->EnumDevices((LPD3DENUMDEVICESCALLBACK)DriverEnumCallback, nullptr);
    if (ret != S_OK) {
        std::cerr << "Failed to enumreate d3d devices\n";
        return -1;
    }

    DDSURFACEDESC modeDesc = {};
    modeDesc.dwSize = sizeof(DDSURFACEDESC);
    ret = lpDD->GetDisplayMode(&modeDesc);
    if (ret != S_OK) {
        std::cerr << "Failed to query current mode\n";
    } else {
        std::cerr << "Current mode: " << modeDesc.dwWidth << " x " << modeDesc.dwHeight << "\n";
        currentWidth = modeDesc.dwWidth;
        currentHeight = modeDesc.dwHeight;
    }

    if (window->CreateWindow(currentWidth, currentHeight)) {
        std::cout << "Width x Height: " << window->GetWindowWidth() << "x" << window->GetWindowHeight() << "\n";
    } else {
        std::cout << "Failed to create native window\n";
        return -1;
    }

    if (!eventLoop.InitializeEventLoop()) {
        std::cerr << "Failed to initialize event loop\n";
        return -1;
    }

    HWND *hWnd = reinterpret_cast<HWND *>(window->GetWindowHandle());
    if (hWnd == nullptr) {
        std::cerr << "Failed to query hWnd\n";
        return -1;
    }

    ret = lpDD->SetCooperativeLevel(*hWnd, DDSCL_NORMAL);
    if (ret != S_OK) {
        std::cerr << "Failed to set cooperative level\n";
        return -1;
    }

    RECT rect = {};
    /*
    POINT point = {};

    if (!ClientToScreen(*hWnd, &point)) {
        std::cerr << "Failed to get screen coordinates\n";
        return -1;
    }
    */
    if (!GetClientRect(*hWnd, &rect)) {
        std::cerr << "Failed to get client rects\n";
        return -1;
    }

    std::cout << "Rect.left: " << rect.left << " Rect.top: " << rect.top << " Rect.right: " << rect.right << " Rect.bottom: " << rect.bottom << "\n";
    LPDIRECTDRAWSURFACE surface = {};
    DDSURFACEDESC surfaceDesc = {};
    surfaceDesc.dwSize = sizeof(surfaceDesc);
    surfaceDesc.dwFlags = DDSD_CAPS;
    surfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    ret = lpDD->CreateSurface(&surfaceDesc, &surface, nullptr);
    if (ret != S_OK) {
        std::cerr << "Failed to create primary surface\n";
        return -1;
    }

    LPDIRECTDRAWSURFACE backSurface = {};
    surfaceDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    surfaceDesc.dwWidth = rect.right;
    surfaceDesc.dwHeight = rect.bottom;
    surfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
    ret = lpDD->CreateSurface(&surfaceDesc, &backSurface, nullptr);
    if (ret != S_OK) {
        std::cerr << "Failed to create back surface\n";
        return -1;
    }

    surfaceDesc = {};
    surfaceDesc.dwSize = sizeof(surfaceDesc);
    ret = backSurface->GetSurfaceDesc(&surfaceDesc);
    if (ret != S_OK) {
        std::cerr << "Failed to get back surface desc\n";
        return -1;
    }

    if (surfaceDesc.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) {
        std::cerr << "Paletted\n";
    }
	
	surfaceDesc = {};
	surfaceDesc.dwSize = sizeof(surfaceDesc);
	surfaceDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_ZBUFFERBITDEPTH;
	surfaceDesc.dwWidth = rect.right;
	surfaceDesc.dwHeight = rect.bottom;
	surfaceDesc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
	if (zBufferDepth & DDBD_32) {
		std::cerr << "DDBD_32\n";
		surfaceDesc.dwZBufferBitDepth = 32;
	} else if (zBufferDepth & DDBD_24) {
		std::cerr << "DDBD_24\n";
		surfaceDesc.dwZBufferBitDepth = 24;
	} else if (zBufferDepth & DDBD_16) {
		std::cerr << "DDBD_16\n";
		surfaceDesc.dwZBufferBitDepth = 16;
	} else if (zBufferDepth & DDBD_8) {
		std::cerr << "DDBD_8\n";
		surfaceDesc.dwZBufferBitDepth = 8;
	} else {
		std::cerr << "HAL requires unsupported Z buffer depth\n";
		return -1;
	}

	LPDIRECTDRAWSURFACE zBuffer = {};
	ret = lpDD->CreateSurface(&surfaceDesc, &zBuffer, nullptr);
    if (ret != S_OK) {
        std::cerr << "Failed to create Z buffer\n";
        return -1;
    }

    LPDIRECT3DDEVICE lpD3DDevice = nullptr;

	ret = backSurface->AddAttachedSurface(zBuffer);
	if (ret != S_OK) {
		std::cerr << "Failed to bind zBuffer to back buffer\n";
		return -1;
	}

	surfaceDesc = {};
	surfaceDesc.dwSize = sizeof(surfaceDesc);
	
	ret = zBuffer->GetSurfaceDesc(&surfaceDesc);
	if (ret != S_OK) {
		std::cerr << "Failed to return surface desc\n";
		return -1;
	} else {
		if (surfaceDesc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
			std::cerr << "Zbuffer inside vidmem\n";
		} else {
			std::cerr << "Zbuffer inside sysmem\n";
		}
	}

    ret = backSurface->QueryInterface(*adapterGUID[2], (LPVOID *)&lpD3DDevice);
    if (ret != S_OK) {
        std::cerr << "Failed to create d3ddevice \n";
        std::cerr << "Ret: " << std::hex << ret << "\n";
        return -1;
    } else {
		std::cout << "Created D3D Device\n";
	}
	
	LPDIRECT3DVIEWPORT lpViewport = nullptr;
	ret = lpD3D->CreateViewport(&lpViewport, NULL);
	if (ret != S_OK) {
		std::cerr << "Failed to create a viewport\n";
		return -1;
	} else {
		std::cerr << "Created a viewport\n";
	}
	
	ret = lpD3DDevice->AddViewport(lpViewport);
	if (ret != S_OK) {
		std::cerr << "Failed to add viewport to d3d device\n";
		return -1;
	} else {
		std::cerr << "Added viewport to d3d device\n";
	}
	
	D3DVIEWPORT viewport = {};
	viewport.dwSize = sizeof(D3DVIEWPORT);
	viewport.dwX = viewport.dwY = 0;
	viewport.dwWidth = currentWidth;
	viewport.dwHeight = currentHeight;
	viewport.dvScaleX = currentWidth / (float)2.0f;
	viewport.dvScaleY = currentHeight / (float)2.0f;
	viewport.dvMaxX = (float)D3DDivide(D3DVAL(currentWidth), D3DVAL(2 * viewport.dvScaleX));
	viewport.dvMaxY = (float)D3DDivide(D3DVAL(currentHeight), D3DVAL(2 * viewport.dvScaleY));
	
	ret = lpViewport->SetViewport(&viewport);
	if (ret != S_OK) {
		std::cerr << "Failed to set viewport values\n";
		return -1;
	} else {
		std::cerr << "Set viewport values\n";
	}
	
	LPDIRECT3DMATERIAL lpMaterial = nullptr;
	ret = lpD3D->CreateMaterial(&lpMaterial, nullptr);
	if (ret != S_OK) {
		std::cerr << "Failed to create material object\n";
		return -1;
	} else {
		std::cerr << "Created material object\n";
	}
	
	D3DMATERIAL mat = {};
	mat.dwSize = sizeof(D3DMATERIAL);
	mat.diffuse.r = (D3DVALUE)1.0;
	mat.diffuse.g = (D3DVALUE)1.0;
	mat.diffuse.b = (D3DVALUE)1.0;
	mat.diffuse.a = (D3DVALUE)1.0;
	mat.ambient.r = (D3DVALUE)1.0;
	mat.ambient.g = (D3DVALUE)1.0;
	mat.ambient.b = (D3DVALUE)1.0;
	mat.ambient.a = (D3DVALUE)1.0;
	mat.specular.r = (D3DVALUE)1.0;
	mat.specular.g = (D3DVALUE)1.0;
	mat.specular.b = (D3DVALUE)1.0;
	mat.power = (float)40.0;
	mat.dwRampSize = 16;
	
	
	ret = lpMaterial->SetMaterial(&mat);
	if (ret != S_OK) {
		std::cerr << "Failed to set material object\n";
		return -1;
	} else {
		std::cerr << "Set material object\n";
	}
	
	D3DMATERIALHANDLE matHdl = {};
	ret = lpMaterial->GetHandle(lpD3DDevice, &matHdl);
	if (ret != S_OK) {
		std::cerr << "Failed to create material handle\n";
		return -1;
	} else {
		std::cerr << "Created material handle\n";
	}
	
	LPDIRECT3DMATERIAL lpBmat = nullptr;
	ret = lpD3D->CreateMaterial(&lpBmat, nullptr);
	if (ret != S_OK) {
		std::cerr << "Failed to create b material\n";
		return -1;
	} else {
		std::cerr << "Created b material\n";
	}
	D3DMATERIAL bMat = {};
	bMat.dwSize = sizeof(D3DMATERIAL);
	bMat.diffuse.r = bMat.diffuse.g = bMat.diffuse.b = bMat.ambient.r = bMat.ambient.g = bMat.ambient.b = 1.0;
	bMat.dwRampSize = 1;
	ret = lpBmat->SetMaterial(&bMat);
	if (ret != S_OK) {
		std::cerr << "Failed to set b material object\n";
		return -1;
	} else {
		std::cerr << "Set b material object\n";
	}
	
	D3DMATERIALHANDLE bMatHdl = {};
	ret = lpBmat->GetHandle(lpD3DDevice, &bMatHdl);
	if (ret != S_OK) {
		std::cerr << "Failed to create b material handle\n";
		return -1;
	} else {
		std::cerr << "Queried b material handle\n";
	}
	
	
	ret = lpViewport->SetBackground(bMatHdl);
	if (ret != S_OK) {
		std::cerr << "Failed to set viewport background\n";
		std::cerr << std::hex << "Err: " << ret << std::dec << "\n";
		return -1;
	} else {
		std::cerr << "Set viewport background\n";
	}
	
	constexpr uint32_t numVertices = 3U;
	D3DTLVERTEX vertices[numVertices] = {};
	vertices[0].sx = D3DVAL(10.0);
	vertices[0].sy = D3DVAL(10.0);
	vertices[0].sz = D3DVAL(0.1);
	vertices[0].rhw = D3DVAL(1.0);
	vertices[0].color = RGBA_MAKE(255, 0, 255, 255);
	vertices[0].specular = RGB_MAKE(0, 0, 255);
	vertices[0].tu = D3DVAL(0.0);
	vertices[0].tv = D3DVAL(0.0);
	vertices[1].sx = D3DVAL(300.0);
	vertices[1].sy = D3DVAL(50.0);
	vertices[1].sz = D3DVAL(0.9);
	vertices[1].rhw = D3DVAL(2.0);
	vertices[1].color = RGBA_MAKE(255, 255, 255, 255);
	vertices[1].specular = RGB_MAKE(0, 0, 0);
	vertices[1].tu = D3DVAL(0.0);
	vertices[1].tv = D3DVAL(0.0);
	vertices[2].sx = D3DVAL(150.0);
	vertices[2].sy = D3DVAL(180.0);
	vertices[2].sz = D3DVAL(0.6);
	vertices[2].rhw = D3DVAL(1.0);
	vertices[2].color = RGBA_MAKE(255, 255, 0, 255);
	vertices[2].specular = RGB_MAKE(0, 0, 0);
	vertices[2].tu = D3DVAL(0.0);
	vertices[2].tv = D3DVAL(0.0);
	
	size_t size = {};
	size = sizeof(D3DVERTEX) * numVertices;
	size += sizeof(D3DINSTRUCTION) * 6;
	size += sizeof(D3DSTATE) * 2;
	size += sizeof(D3DPROCESSVERTICES);
	size += sizeof(D3DTRIANGLE) * 1;
	
	D3DEXECUTEBUFFERDESC executeBufferDesc = {};
	LPDIRECT3DEXECUTEBUFFER lpExecBuffer = nullptr;
	executeBufferDesc.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
	executeBufferDesc.dwFlags = D3DDEB_BUFSIZE;
	executeBufferDesc.dwBufferSize = size;
	ret = lpD3DDevice->CreateExecuteBuffer(&executeBufferDesc, &lpExecBuffer, nullptr);
	if (ret != S_OK) {
		std::cerr << "Failed to create execute buffer\n";
		return -1;
	} else {
		std::cerr << "Created execute buffer\n";
	}
	
	ret = lpExecBuffer->Lock(&executeBufferDesc);
	if (ret != S_OK) {
		std::cerr << "Failed to lock execute buffer\n";
		return -1;
	} else {
		std::cerr << "Locked execute buffer\n";
	}
	
	LPVOID lpBufStart, lpInsStart, lpPointer;
	lpBufStart = executeBufferDesc.lpData;
	memset(lpBufStart, 0, size);
	lpPointer = lpBufStart;

    memcpy(lpPointer, &vertices[0], sizeof(D3DVERTEX) * numVertices);
	lpPointer = (void *)(((LPD3DVERTEX) (lpPointer)) + (sizeof(D3DVERTEX) * numVertices));
	lpInsStart = lpPointer;
	
	#define PUTD3DINSTRUCTION(op, sz, cnt, ptr) \
    ((LPD3DINSTRUCTION) ptr)->bOpcode = op; \
    ((LPD3DINSTRUCTION) ptr)->bSize = sz; \
    ((LPD3DINSTRUCTION) ptr)->wCount = cnt; \
    ptr = (void *)(((LPD3DINSTRUCTION) ptr) + 1)
	D3DINSTRUCTION instruction = {};
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->bOpcode = D3DOP_STATELIGHT;
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->bSize = sizeof(D3DSTATE);
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->wCount = 1;
	lpPointer = (void *)(((LPD3DINSTRUCTION) lpPointer) + 1);

	
	reinterpret_cast<LPD3DSTATE>(lpPointer)->drstRenderStateType = (D3DRENDERSTATETYPE)D3DLIGHTSTATE_MATERIAL;
	reinterpret_cast<LPD3DSTATE>(lpPointer)->dwArg[0] = matHdl;
	lpPointer = (void *)(((LPD3DSTATE) lpPointer) + 1);
	
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->bOpcode = D3DOP_PROCESSVERTICES;
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->bSize = sizeof(D3DPROCESSVERTICES);
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->wCount = 1;
	lpPointer = (void *)(((LPD3DINSTRUCTION)lpPointer) + 1);
	
	reinterpret_cast<LPD3DPROCESSVERTICES>(lpPointer)->dwFlags = D3DPROCESSVERTICES_COPY | D3DPROCESSVERTICES_UPDATEEXTENTS;
	reinterpret_cast<LPD3DPROCESSVERTICES>(lpPointer)->wStart = 0;
	reinterpret_cast<LPD3DPROCESSVERTICES>(lpPointer)->wDest = 0;
	reinterpret_cast<LPD3DPROCESSVERTICES>(lpPointer)->dwCount = numVertices;
	reinterpret_cast<LPD3DPROCESSVERTICES>(lpPointer)->dwReserved = 0;
	lpPointer = (void *)(((LPD3DPROCESSVERTICES)lpPointer) + 1);
	
	#define QWORD_ALIGNED(ptr) \
    !(0x00000007L & (ULONG)(ptr))
	if (QWORD_ALIGNED(lpPointer)) {
		reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->bOpcode = D3DOP_TRIANGLE;
		reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->bSize = sizeof(D3DTRIANGLE);
		reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->wCount = 0;
		lpPointer = (void *)(((LPD3DINSTRUCTION)lpPointer) + 1);
    }
	
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->bOpcode = D3DOP_TRIANGLE;
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->bSize = sizeof(D3DTRIANGLE);
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->wCount = 1;
	lpPointer = (void *)(((LPD3DINSTRUCTION) lpPointer) + 1);
	
	reinterpret_cast<LPD3DTRIANGLE>(lpPointer)->v1 = 0;
	reinterpret_cast<LPD3DTRIANGLE>(lpPointer)->v2 = 0;
	reinterpret_cast<LPD3DTRIANGLE>(lpPointer)->v3 = 0;
	reinterpret_cast<LPD3DTRIANGLE>(lpPointer)->wFlags = D3DTRIFLAG_EDGEENABLETRIANGLE;
	lpPointer = ((char*)lpPointer) + sizeof(D3DTRIANGLE);
	
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->bOpcode = D3DOP_EXIT;
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->bSize = 0;
	reinterpret_cast<LPD3DINSTRUCTION>(lpPointer)->wCount = 0;
	
	ret = lpExecBuffer->Unlock();
	if (ret != S_OK) {
		std::cerr << "Failed to unlock execute buffer\n";
		return -1;
	} else {
		std::cerr << "Unlocked execute buffer\n";
	}
	
	D3DEXECUTEDATA d3dExData = {};
	d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
	d3dExData.dwVertexCount = numVertices;
	d3dExData.dwInstructionOffset = (ULONG) ((char *)lpInsStart - (char *)lpBufStart);
	d3dExData.dwInstructionLength = (ULONG) ((char *)lpPointer - (char *)lpInsStart);
	
	ret = lpExecBuffer->SetExecuteData(&d3dExData);
	if (ret != S_OK) {
		std::cerr << "Failed to set execute data\n";
		return -1;
	} else {
		std::cerr << "Execute data set\n";
	}
	
	
	
	
	
	
		
	/* Texture stuff. Will get to this later
	ret = lpD3DDevice->EnumTextureFormats((LPD3DENUMTEXTUREFORMATSCALLBACK)TextureFormatEnumCallback, nullptr);
	if (ret != S_OK) {
		std::cerr << "Failed to enumerate texture formats\n";
		return -1;
	}*/

    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
    }
}
