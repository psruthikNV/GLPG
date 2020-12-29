#include <iostream>
#include <strsafe.h>

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <dxgi.h>
#include <wrl/client.h>
#include "d3dx12.h" //TODO: Remove dependency on this header
#include <random>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "user32.lib")
#pragma warning(disable:4996)

template <typename T>
class counter {
public:
	counter(T val) : m_val(val), performAddition(true) {};
	counter() : m_val(T(0)), performAddition(true) {};

	// Perhaps the most stupidest op overload ever.
	T operator++() {
		if (performAddition) {
			m_val += 0.001;
		}
		else {
			m_val -= 0.001;
		}

		if (m_val > T(1) || m_val <= T(0)) {
			performAddition = !performAddition;
		}
		return m_val;
	}

private:
	T m_val;
	bool performAddition;
};

using counter_f = counter<float>;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dist(0.0, 1.0);
counter_f red(dist(gen));
counter_f green(dist(gen));
counter_f blue(dist(gen));
float colors[4];

using Microsoft::WRL::ComPtr;

// Global D3D12 interface handles
ComPtr<IDXGIFactory4> factory;
ComPtr<IDXGIAdapter1> adapter;
ComPtr<ID3D12Device> device;
ComPtr<ID3D12CommandQueue> commandQueue;
ComPtr<IDXGISwapChain1> swapChain;
ComPtr<IDXGISwapChain3> m_swapChain;
ComPtr<ID3D12DescriptorHeap> descHeap;
ComPtr<ID3D12Resource> renderTarget[2];
ComPtr<ID3D12CommandAllocator> commandAllocator;
ComPtr<ID3D12GraphicsCommandList> commandList;
ComPtr<ID3D12Fence> fence;
ComPtr<ID3D12PipelineState> pipelineState;
HANDLE fenceEvent;

UINT descriptorSize = 0;
static int frameIdx;
static int fenceValue;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int setupD3D12(HWND* hWnd)
{
	int dxgiFactoryFlags = 0;

	// Initialize Debug Layer
	ComPtr<ID3D12Debug> debugController;

	if (D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)) != 0) {
		OutputDebugString(L"\n Failed to Get Debug Interface");
		return -1;
	}
	debugController->EnableDebugLayer();
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

	// Create the DXGI Factory object

	if (CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)) != 0) {
		OutputDebugString(L"\n Failed to Create DXGI Factory");
		return -1;
	}

	ComPtr<IDXGIAdapter1> tempAdapter;

	for (int adapterIndex = 0; factory->EnumAdapters1(adapterIndex, &tempAdapter) != DXGI_ERROR_NOT_FOUND; adapterIndex++) {
		DXGI_ADAPTER_DESC1 desc;
		tempAdapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if (D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr) == 0) {
			break;
		}
		adapter = tempAdapter.Detach();
	}

	if (D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)) != 0) {
		OutputDebugString(L"\n Failed to Create D3D12 Device");
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};

	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)) < 0) {
		OutputDebugString(L"Failed to create Command Queue");
	}


	DXGI_SWAP_CHAIN_DESC1 swapDesc{};
	swapDesc.BufferCount = 2;
	swapDesc.Width = 1920;
	swapDesc.Height = 1080;
	swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // Discard the contents of the back buffer after a present call
	swapDesc.SampleDesc.Count = 1; // Specifies the multisampling count 

	// For d3d11, commandQueue is replaced with a handle to the d3d11 device.
	if (factory->CreateSwapChainForHwnd(commandQueue.Get(), *hWnd, &swapDesc, nullptr, nullptr,
		&swapChain) < 0) {
		OutputDebugString(L"\n Failed to create Swap Chain");
	}

	// Allows DXGI to handle alt-enter sequences for switching from window to full screen
	if (factory->MakeWindowAssociation(*hWnd, DXGI_MWA_NO_ALT_ENTER)) {
		OutputDebugString(L"\n Failed to make window association");
	}

	if (swapChain.As(&m_swapChain) < 0) {
		printf("\n Failed to set swap chain");
	}
	frameIdx = m_swapChain->GetCurrentBackBufferIndex();

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};

	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //descriptor for Render target view
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&descHeap)) < 0) {
		OutputDebugString(L"Failed to create descriptor heap");
	}
	descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descHeap->GetCPUDescriptorHandleForHeapStart()); // CPU handle for the descHeap
																							 // Generic handle for both CPU and GPU to read and write to memory

	for (int i = 0; i < 2; i++) {
		if (m_swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i])) < 0)
			OutputDebugString(L"\n Failed to get Buffer from swap chain");
		device->CreateRenderTargetView(renderTarget[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, descriptorSize);
	}


	if (device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)) < 0)
		OutputDebugString(L"\n Failed to create allocation for commands");


	if (device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)) < 0) {
		printf("\n Failed to Create Command list");
	}

	if (commandList->Close() < 0) {
		printf("\n Failed to Close command list");
	}




	if (device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)) < 0) {
		printf("\n Fained to Create Fence object");
	}

	fenceValue = 1;

	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr); // ?
	if (fenceEvent == nullptr) {
		printf("\n Failed to Create Fence Event");
	}
	return 0;
}

int render()
{
	colors[3] = 0.0F;
	colors[0] = red.operator++();
	colors[1] = green.operator++();
	colors[2] = blue.operator++();
	//frameIdx = (frameIdx == 1) ? 0 : frameIdx;
	if (commandAllocator->Reset() < 0) {
		printf("\n Failed to reset Command Allocator");
		return -1;
	}

	if (commandList->Reset(commandAllocator.Get(), pipelineState.Get()) < 0) {
		printf("\n Failed to reset Command List");
		return -1;
	}

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget[frameIdx].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descHeap->GetCPUDescriptorHandleForHeapStart(), frameIdx, descriptorSize);

	//const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, colors, 0, nullptr);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget[frameIdx].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	if (commandList->Close() < 0) {
		printf("\n Failed to close command list");
		return -1;
	}

	ID3D12CommandList* ppCommandList[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

	if (m_swapChain->Present(0, 0) < 0) {
		printf("\n Failed to present the frame");
		return -1;
	}


	const UINT64 fence = fenceValue;
	if (commandQueue->Signal(::fence.Get(), fence) < 0) {
		printf("\n Failed to signal fence");
		return -1;
	}
	fenceValue++;

	if (::fence->GetCompletedValue() < fence) {
		if (::fence->SetEventOnCompletion(fence, fenceEvent) < 0) {
			printf("\n Failed to set event on completion");
			return -1;
		}
		WaitForSingleObject(fenceEvent, INFINITE);
	}
	frameIdx = m_swapChain->GetCurrentBackBufferIndex();
	return 0;
}
int main()
{
	ATOM ret;
	WNDCLASSEX wClass = { 0 };
	HWND hWnd;
	HINSTANCE hInstance = GetModuleHandle(NULL);


	AllocConsole();

	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	wClass.cbSize = sizeof(WNDCLASSEX);

	// Redraw on vertical and horizontal changes to the 
	// client area of the window. 
	wClass.style = CS_HREDRAW | CS_VREDRAW;

	wClass.lpfnWndProc = WindowProc; // Shared by all windows created using this class
	wClass.hInstance = hInstance;

	// Load a cursor from the specified hInstance's exe. (NULL here)
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	wClass.lpszClassName = L"DX12Class";

	ret = RegisterClassEx(&wClass);

	// The provided width and height are meant only for the
	// dimensions of the client area. Use those dimensions 
	// for generating the width and height of the entire window
	RECT windowRect = { 0, 0, 640, 480 };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindow(wClass.lpszClassName, L"DX12 stuff", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);


	int err = setupD3D12(&hWnd);

	ShowWindow(hWnd, SW_SHOWNORMAL);

	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		render();
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}