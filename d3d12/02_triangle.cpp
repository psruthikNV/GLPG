#include <iostream>
#include <vector>
#include <random>
#include <fstream>

#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"
#include "../include/GLPGD3D12Context.hpp"

#include "d3dcompiler.h"

#undef CreateWindow // We require this since GLPG::CreateWindow collides with the Win32 API

template <typename T>
class counter {
public:
    counter(T val) : m_val(val), performAddition(true) {};
    counter() : m_val(T(0)), performAddition(true) {};

    // Perhaps the most stupidest op overload ever.
    T operator++() {
        if (performAddition) {
            m_val += T(0.001);
        } else {
            m_val -= T(0.001);
        }

        if (m_val > T(1)|| m_val <= T(0)) {
            performAddition = !performAddition;
        }
        return m_val;
    }

private:
    T m_val;
    bool performAddition;
};

using counter_f = counter<float>;
using Microsoft::WRL::ComPtr;

int main() {
    HRESULT ret;
    uint32_t idx = 0U;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;
    counter_f red(dist(gen));
    counter_f green(dist(gen));
    counter_f blue(dist(gen));

    GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    if (!window) {
        std::cerr << "Failed to create GLPGWindow\n";
        return -1;
    }

    GLPG::GLPGD3D12Context gc;
    if (!gc.inited) {
        std::cerr << "Failed to initialize D3D12\n";
        return -1;
    }

    if (window->CreateWindow(gc.defaultWidth, gc.defaultHeight)) {
        std::cout << "Width x Height: " << window->GetWindowWidth() << "x" << window->GetWindowHeight() << "\n";
    } else {
        std::cout << "Failed to create native window\n";
        return -1;
    }

    if (!gc.InitializeSwapchain()) {
        std::cerr << "Failed to initialize swapchain\n";
        return -1;
    }

    if (!eventLoop.InitializeEventLoop()) {
        std::cerr << "Failed to initialize event loop\n";
        return -1;
    }   

    uint32_t backBufferIdx = gc.pSwapchain3->GetCurrentBackBufferIndex();

    D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeap = {};
    rtvDescHeap.NumDescriptors = 2;
    rtvDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ID3D12DescriptorHeap *pRtvDescHeap;
    ret = gc.pD3d12Device->CreateDescriptorHeap(&rtvDescHeap, IID_PPV_ARGS(&pRtvDescHeap));
    if (ret != S_OK) {
        std::cerr << "Failed to create RTV Descriptor heap\n";
    }

    D3D12_CPU_DESCRIPTOR_HANDLE rtvDescHandle = pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
    uint32_t rtvDescIncrementSize = gc.pD3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    ComPtr<ID3D12Resource> pswapchainBuffers[2];
    for (uint32_t idx = 0; idx < 2; idx++) {
        ret = gc.pSwapchain3->GetBuffer(idx, IID_PPV_ARGS(&pswapchainBuffers[idx]));
        if (ret != S_OK) {
            std::cerr << "Failed to get buffer\n";
        } else {
            gc.pD3d12Device->CreateRenderTargetView(pswapchainBuffers[idx].Get(), nullptr, rtvDescHandle);
            rtvDescHandle.ptr += rtvDescIncrementSize;
        }
    }
    if (ret != S_OK) {
        std::cerr << "Failed to create RTVs for swapchain buffers\n";
        return -1;
    }

    // Create an empty root signature
    D3D12_ROOT_SIGNATURE_DESC  initialNullRootSignatureDesc = {};
    initialNullRootSignatureDesc.NumParameters = 0;
    initialNullRootSignatureDesc.pParameters = nullptr;
    initialNullRootSignatureDesc.NumStaticSamplers = 0;
    initialNullRootSignatureDesc.pStaticSamplers = 0;
    initialNullRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC initialNullVersionedRootSignatureDesc = {};
    initialNullVersionedRootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1;
    initialNullVersionedRootSignatureDesc.Desc_1_0 = initialNullRootSignatureDesc; 

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ComPtr<ID3D12RootSignature> rootSignature;
    ret = D3D12SerializeVersionedRootSignature(&initialNullVersionedRootSignatureDesc, &signature, &error);
    if (ret != S_OK) {
        std::cerr << "Failed to serialize Root signature\n";
        return -1;
    }

    ret = gc.pD3d12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),  IID_PPV_ARGS(&rootSignature));
    if (ret != S_OK) {
        std::cerr << "Failed to create root signature\n";
        return -1;
    }

    // Create pipeline state
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;
    char *vsCompiledBlob;
    char *psCompiledBlob;
    D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
    D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
    const char *compiledVSPath = "C:/repos/GLPG/build/d3d12/02_triangle_vs";
    const char *compiledPSPath = "C:/repos/GLPG/build/d3d12/02_triangle_ps";
    std::fstream fs;
    std::streampos vsCompiledBlobSize;
    std::streampos psCompiledBlobSize;

    fs.open(compiledVSPath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!fs.is_open()) {
        std::cerr << "Failed to open vso file\n";
    } else {
        vsCompiledBlobSize = fs.tellg();
        vsCompiledBlob = new char[vsCompiledBlobSize];
        fs.seekg(0, std::ios::beg);
        fs.read((char *)(vsCompiledBlob), vsCompiledBlobSize);
        fs.close();
    }

    fs.open(compiledPSPath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!fs.is_open()) {
        std::cerr << "Failed to open vso file\n";
    } else {
        psCompiledBlobSize = fs.tellg();
        psCompiledBlob = new char[psCompiledBlobSize];
        fs.seekg(0, std::ios::beg);
        fs.read((char *)(psCompiledBlob), psCompiledBlobSize);
        fs.close();
    }

    vertexShaderBytecode.pShaderBytecode = vsCompiledBlob; 
    vertexShaderBytecode.BytecodeLength = vsCompiledBlobSize;
    pixelShaderBytecode.pShaderBytecode = psCompiledBlob;
    pixelShaderBytecode.BytecodeLength = psCompiledBlobSize;

    D3D12_INPUT_ELEMENT_DESC iaDesc = {
        "POSITION",
        0,
        DXGI_FORMAT_R32G32B32_FLOAT,
        0,
        0,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
        0
    };

    D3D12_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthClipEnable = true;
    rasterizerDesc.MultisampleEnable = false;
    rasterizerDesc.AntialiasedLineEnable = false;
    rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizerDesc.ForcedSampleCount = 0;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
    inputLayoutDesc.pInputElementDescs = &iaDesc;
    inputLayoutDesc.NumElements = 1U;

    D3D12_RENDER_TARGET_BLEND_DESC  renderTargetBlendDesc = {};
    renderTargetBlendDesc.BlendEnable = false;
    renderTargetBlendDesc.LogicOpEnable = false;
    renderTargetBlendDesc.SrcBlend = D3D12_BLEND_ONE;
    renderTargetBlendDesc.DestBlend = D3D12_BLEND_ZERO;
    renderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
    renderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
    renderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
    renderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    renderTargetBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
    renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0] = renderTargetBlendDesc;

    ComPtr<ID3D12PipelineState> graphicsPipelineState;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS = vertexShaderBytecode; 
    psoDesc.PS = pixelShaderBytecode;
    psoDesc.RasterizerState = rasterizerDesc;
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.InputLayout = inputLayoutDesc; 
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.BlendState = blendDesc;
    psoDesc.SampleMask = 1;

    ret = gc.pD3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&graphicsPipelineState));
    if (ret != S_OK) {
        std::cerr << "Failed to create graphics pipeline state\n";
        return -1;
    }

    if (!gc.InitializeCommandQueueResources(graphicsPipelineState.Get())) {
        return -1;
    }

    // Create the vertex buffer.
    struct VtxInput {
        float x;
        float y;
        float z;
    };

    VtxInput vertexBufferInput[3] = {
        {-0.5, -0.5, 0.0},
        {0.0, 0.5, 0.0},
        {0.5, -0.5, 0.0}
    };
    const uint32_t vertexBufferSize = sizeof(vertexBufferInput);

    D3D12_HEAP_PROPERTIES vertexBufferHeapProp = {};
    vertexBufferHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

    DXGI_SAMPLE_DESC sampleDesc = { 0 };
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    D3D12_RESOURCE_DESC vertexBufferDesc = {};
    vertexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    vertexBufferDesc.Width = vertexBufferSize;
    vertexBufferDesc.Height = 1;
    vertexBufferDesc.DepthOrArraySize = 1;
    vertexBufferDesc.MipLevels = 1;
    vertexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    vertexBufferDesc.SampleDesc = sampleDesc;
    vertexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    vertexBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ComPtr<ID3D12Resource> vertexBuffer;
    ret = gc.pD3d12Device->CreateCommittedResource(&vertexBufferHeapProp, D3D12_HEAP_FLAG_NONE,
                                                   &vertexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                                   nullptr, IID_PPV_ARGS(&vertexBuffer));
    if (ret != S_OK) {
        std::cerr << "Failed to create vertex buffer\n";
        return -1;
    }

    // Copy the triangle data to the vertex buffer
    uint8_t *vertexBufferCpuVAPtr;
    D3D12_RANGE vertexBufferReadRange = {};

    ret = vertexBuffer->Map(0, &vertexBufferReadRange, reinterpret_cast<void**>(&vertexBufferCpuVAPtr));
    if (ret != S_OK) {
        std::cerr << "Failed to map vertex buffer to CPU VA\n";
        return -1;
    }
    memcpy(vertexBufferCpuVAPtr, vertexBufferInput, sizeof(vertexBufferInput));
    vertexBuffer->Unmap(0, nullptr);

    // Initialize the vertex buffer view
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(VtxInput);
    vertexBufferView.SizeInBytes = vertexBufferSize;

    ComPtr<ID3D12Fence> pFence;
    uint32_t initialFenceValue = 0U;
    uint32_t signaledFenceValue = initialFenceValue + 1U;
    ret = gc.pD3d12Device->CreateFence(initialFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
    if (ret != S_OK) {
        std::cerr << "Failed to create D3d12 fence\n";
        return -1;
    }

    // Create an unsignaled event
    HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fenceEvent == nullptr) {
        std::cerr << "Failed to create event\n";
        return -1;
    }

    // Wait for vertex buffer to be uploaded to the GPU.
    ret = gc.pCommandQueue->Signal(pFence.Get(), signaledFenceValue);
    if (ret != S_OK) {
        std::cerr << "Failed to signal fence\n";
        return -1;
    }

    if (pFence->GetCompletedValue() < signaledFenceValue) {
        if (pFence->SetEventOnCompletion(signaledFenceValue, fenceEvent) != S_OK) {
            std::cerr << "failed to set event on completion\n";
            return -1;
        }
        WaitForSingleObject(fenceEvent, INFINITE);
    }
    //backBufferIdx = gc.pSwapchain3->GetCurrentBackBufferIndex();
    signaledFenceValue++;

    float colors[4];
    colors[3] = 0.0F;

    D3D12_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(gc.defaultWidth);
    viewport.Height = static_cast<float>(gc.defaultHeight);

    D3D12_RECT scissorRect = {};
    scissorRect.right = gc.defaultWidth;
    scissorRect.bottom = gc.defaultHeight;

    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
	    colors[0] = red.operator++();
	    colors[1] = green.operator++();
	    colors[2] = blue.operator++();

        ret = gc.pCmdAllocator->Reset();
        if (ret != S_OK) {
            std::cerr << "Failed to reset command allocator: " << std::hex << ret << std::dec << "\n" ;
            return -1;
        }

        ret = gc.pCmdList->Reset(gc.pCmdAllocator.Get(), graphicsPipelineState.Get());
        if (ret != S_OK) {
            std::cerr << "failed to reset command list\n";
            return -1;
        }

        gc.pCmdList->SetGraphicsRootSignature(rootSignature.Get());
        gc.pCmdList->RSSetViewports(1, &viewport);
        gc.pCmdList->RSSetScissorRects(1,&scissorRect);

        D3D12_RESOURCE_TRANSITION_BARRIER rtvPreTransationBarrier = {
            pswapchainBuffers[backBufferIdx].Get(),
            D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        };

        D3D12_RESOURCE_BARRIER rtvPreBarrier = {};
        rtvPreBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        rtvPreBarrier.Transition = rtvPreTransationBarrier;
        rtvPreBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

        gc.pCmdList->ResourceBarrier(1, &rtvPreBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE currentRtvHandle = pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        currentRtvHandle.ptr += (backBufferIdx * rtvDescIncrementSize);

        gc.pCmdList->OMSetRenderTargets(1, &currentRtvHandle, false, nullptr);

        gc.pCmdList->ClearRenderTargetView(currentRtvHandle, colors, 0, nullptr);
        gc.pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        gc.pCmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
        gc.pCmdList->DrawInstanced(3, 1, 0, 0);

        D3D12_RESOURCE_TRANSITION_BARRIER rtvPostTransationBarrier = {
            pswapchainBuffers[backBufferIdx].Get(),
            D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
        };
    
        D3D12_RESOURCE_BARRIER rtvPostBarrier = {};
        rtvPostBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        rtvPostBarrier.Transition = rtvPostTransationBarrier;
        rtvPostBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

        gc.pCmdList->ResourceBarrier(1, &rtvPostBarrier);

        if ((ret = gc.pCmdList->Close()) != S_OK) {
            std::cerr << "Failed to close command list " << std::hex << ret << std::dec << "\n" ;
            return -1;
        }

        ID3D12CommandList *ppCmdList;
        ret = gc.pCmdList->QueryInterface(IID_PPV_ARGS(&ppCmdList));
        if (ret != S_OK) {
            std::cerr << "Failed to get cmdlist\n";
            return -1;
        }
        gc.pCommandQueue->ExecuteCommandLists(1U, &ppCmdList);

        ret = gc.pSwapchain3->Present(1, 0);
        if (ret != S_OK) {
            std::cerr << "Failed to present \n";
            return -1;
        }

        ret = gc.pCommandQueue->Signal(pFence.Get(), signaledFenceValue);
        if (ret != S_OK) {
            std::cerr << "Failed to signal fence\n";
            return -1;
        }

        if (pFence->GetCompletedValue() < signaledFenceValue) {
            if (pFence->SetEventOnCompletion(signaledFenceValue, fenceEvent) != S_OK) {
                std::cerr << "failed to set event on completion\n";
                return -1;
            }
            WaitForSingleObject(fenceEvent, INFINITE);
        }
        backBufferIdx = gc.pSwapchain3->GetCurrentBackBufferIndex();
        signaledFenceValue++;
    }
}
