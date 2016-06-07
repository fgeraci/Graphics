#pragma once
#include "pch.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;

namespace Application {
	
	ref class Gfx sealed {

	public:

		Gfx();
		void Draw();
		void ResizeBuffers();
		void Terminate();

		property UINT ClientWidht {
			void set(UINT w) {
				g_ClientWidth = w;
			}
		}
		property UINT ClientHeight {
			void set(UINT h) {
				g_ClientHeight = h;
			}
		}

	private:

		static const bool g_UseGPU = false;
		static const bool g_RestrictToPrimaryOutput = false;
		static const bool m_MsaaEnabled = false;
		static const UINT g_SwapChainBuffersCount = 2;
		static const DXGI_FORMAT g_BackbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT g_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		
		int g_CurrentBackbuffer = 0;
		int g_CurrentMSAaLevel = 4;
		bool g_DebugEnabled = false;
		UINT64 g_CurrentFence = 0;

		DXGI_MODE_DESC g_PrimaryOutput;
		D3D12_VIEWPORT g_ViewPort;
		D3D12_RECT g_ScissorsRectangle;

		ComPtr<ID3D12Debug> g_DebugController;
		ComPtr<IDXGIFactory4> g_DxgiFactory;
		ComPtr<ID3D12Device> g_Device;
		ComPtr<ID3D12Fence>	g_Fence;
		ComPtr<ID3D12CommandQueue> g_CommandQueue;
		ComPtr<ID3D12CommandAllocator> g_CommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> g_CommandList;
		ComPtr<IDXGISwapChain1> g_SwapChain;
		ComPtr<ID3D12DescriptorHeap> g_RtvDescHeap;
		ComPtr<ID3D12DescriptorHeap> g_DsvDescHeap;
		CD3DX12_CPU_DESCRIPTOR_HANDLE g_DsvDesc;
		ComPtr<ID3D12Resource> g_Backbuffers[g_SwapChainBuffersCount];
		ComPtr<ID3D12Resource> g_DepthStencilBuffer;
		
		// Native properties
		UINT g_MsaaQualityLevels;
		UINT g_RtvDescriptorSize, g_DsvDescriptorSize, g_SrvDescriptorSize;
		UINT g_ClientWidth;
		UINT g_ClientHeight;
		
		// Main methods
		void InitializeDebugging();
		void InitializeDirect3D();

		// Utility

		IDXGIAdapter1* GetGPU();
		void GetClientOutputProperties();
		void CheckAntialisngSupport();
		void CreateDescriptorHeaps();
		void CreateDepthStencilBuffer();
		void FlushGPUCommandsQueue();
		ID3D12Resource* GetCurrentBackBuffer();
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentRtv();
		D3D12_CPU_DESCRIPTOR_HANDLE GetDst();
		void CloseCommandList();
		void CreateSwapChain();
	};

}
