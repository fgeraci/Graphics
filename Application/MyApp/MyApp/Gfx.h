#pragma once

#include "pch.h"

using namespace Microsoft::WRL;
using namespace DirectX;

/*
*	What's in here?
*
*	Gfx (graphics) is the main wrapper class for our Direct3D acces.
*	This will: Initialize Debugging, Direct3D, the Pipeline and other utility functions.
*
*	On the side:
*	
*	We can find wrapper auxiliary classes such as UploadBuffer, which encloses an
*	ID3DResource on the UPDATE heap for the CPU to be able to write to every frame.
*/

namespace Application {

	class Gfx sealed {

	public:

		Gfx();

		void Draw();
		void ResizeBuffers();
		void Terminate();

		void ClientWidht(UINT w) {
			g_ClientWidth = w;
		}

		void ClientHeight(UINT h) {
			g_ClientHeight = h;
		}

		Entity* AddPolygon(POLYGON_TYPE);
		Entity* AddPolygon(POLYGON_TYPE, bool dyn);
		Entity* WorldEntityParent() { return g_WorldEntity.get(); }

		// Accessors / Mutators
		Entity* MainCamera();

	private:

		// Default Values
		int g_CurrentBackbuffer = 0;
		int g_CurrentMSAaLevel = 4;
		bool g_DebugEnabled = false;
		UINT64 g_CurrentFence = 0;
		XMVECTORF32 g_BackBufferColor = DirectX::Colors::Black;
		XMFLOAT4 g_MainCameraInitialPosition = DirectX::XMFLOAT4(0.0f, 4.0f, -15.0f, 1.0f);
		static const bool g_UseGPU = false;
		static const bool g_RestrictToPrimaryOutput = false;
			// Multisample is not supported in the swap chain, it need to be created on a separate render target
		static const bool m_MsaaEnabled = false;
		static const UINT g_SwapChainBuffersCount = 2;
		static const DXGI_FORMAT g_BackbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT g_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		
		// Main Scene Objects
		std::unique_ptr<Camera> g_MainCamera = nullptr;
		std::unique_ptr<Grid>	g_Grid = nullptr;
		std::unique_ptr<Entity> g_WorldEntity = nullptr;

		// Buffers and command lists
		UploadBuffer<ObjectConstantData>* g_MainCameraBuffer = nullptr;
		CommandListHelper				  g_EntityCommandList;
		// D3D Resources
			// Fresources will be updated once per draw call and pushed to the CPU. The CPU will be blocked from continuing
			// if the GPU hasn't caught up with it, ideally, GPU utilization has to be maxed out, while CPU can idle.
			// The more FrameResources we have, the further ahead the CPU can process frames and submit CmdAllocators to it.
		std::vector<FrameResources> g_FrameResources;
		int g_CurrentFrameResources = -1;


		ComPtr<ID3D12Resource> g_CPUVertexBuffer = nullptr;
		ComPtr<ID3D12Resource> g_CPUIndexBuffer = nullptr;
		ComPtr<ID3D12Resource> g_GPUVertexBuffer = nullptr;
		ComPtr<ID3D12Resource> g_GPUIndexBuffer = nullptr;

			// main descriptors
		DXGI_MODE_DESC g_PrimaryOutput;
		D3D12_VIEWPORT g_ViewPort;
		D3D12_RECT g_ScissorsRectangle;
			// main interfaces
		ComPtr<ID3D12Debug> g_DebugController;
		ComPtr<IDXGIFactory4> g_DxgiFactory;
		ComPtr<ID3D12Device> g_Device;
		ComPtr<ID3D12Fence>	g_Fence;
		ComPtr<ID3D12CommandQueue> g_CommandQueue;
		ComPtr<ID3D12CommandAllocator> g_CommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> g_CommandList;
		ComPtr<IDXGISwapChain1> g_SwapChain;
		ComPtr<ID3D12DescriptorHeap> g_RtvDescHeap;		// render target descriptors
		ComPtr<ID3D12DescriptorHeap> g_DsvDescHeap;		// Depth/Stencil descriptors
		ComPtr<ID3D12DescriptorHeap> g_CbvDescHeap;		// Constan Buffers descriptors
		CD3DX12_CPU_DESCRIPTOR_HANDLE g_DsvDesc;
		ComPtr<ID3D12Resource> g_Backbuffers[g_SwapChainBuffersCount];
		ComPtr<ID3D12Resource> g_DepthStencilBuffer;
		ComPtr<ID3D12RootSignature> g_RootSignature;
		std::vector<D3D12_INPUT_ELEMENT_DESC> g_InputLayout;
		ComPtr<ID3D12PipelineState> g_PipelineStateTriangle;
		ComPtr<ID3D12PipelineState> g_PipelineStateLine;
		
		// For Draw Calls
		void UpdateCamera();
		void UpdateGrid();
		
		// Native properties
		UINT g_MsaaQualityLevels;
		UINT g_RtvDescriptorSize, g_DsvDescriptorSize, g_SrvDescriptorSize;
		UINT g_ClientWidth;
		UINT g_ClientHeight;
		
		// Main methods
		void InitializeDebugging();
		void InitializeDirect3D();
		void InitializePipeline();
		void InitializeMainCamera();
		void InitializeEntityCommandList();

		// Utility
		IDXGIAdapter1* GetGPU();
		void GetClientOutputProperties();
		void CheckAntialisngSupport();
		void CreateDescriptorHeaps();
		void CreateDepthStencilBuffer();
		void FlushGPUCommandsQueueForFence(UINT64);
		void FlushGPUCommandsQueueAll();
		ID3D12Resource* GetCurrentBackBuffer();
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentRtv();
		D3D12_CPU_DESCRIPTOR_HANDLE GetDsv();
		void CloseGlobalCommandList();
		void CloseEntitiesCommandLists();
		void CreateSwapChain();
		void CreateInputLayout();
		void CreateCameraConstantBuffers();
		void CreateRootSignature();
		void CreatePSO(D3D12_PRIMITIVE_TOPOLOGY_TYPE, ComPtr<ID3D12PipelineState>&);
			// creates a generic buffer - ID3DResource for index and vertex buffers
		ComPtr<ID3D12Resource> CreateDefaultBuffer(UINT,const void*, ComPtr<ID3D12Resource>&);
		ComPtr<ID3DBlob> LoadFileBlob(const std::wstring&);
	};
}
