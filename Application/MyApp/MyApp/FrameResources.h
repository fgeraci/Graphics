#pragma once

#include "pch.h"

using namespace DirectX;
using namespace Application::Enums::Transformation;

namespace Application {
	
	class FrameResources {
	
	private:
		
		UINT g_FrameResourceIndex;
		UINT g_CBVDescriptorHeapIndex;

		ComPtr<ID3D12CommandAllocator> g_CmdAllocator;
		
		// Beginning Fence
		UINT64 g_CurrentFence;
		
		// Resources
		Entity* g_EntityResource;

	public:

		FrameResources(ComPtr<ID3D12Device> device, UINT64 fence, UINT index) {
			g_CurrentFence = fence;
			DX::ThrowIfFailed(device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_CmdAllocator))
			);
			g_FrameResourceIndex = index;
		}

		FrameResources(ComPtr<ID3D12Device> device, UINT64 fence, Entity* e, UINT index) : FrameResources(device,fence,index) {
			g_EntityResource = e;
		}

		ComPtr<ID3D12CommandAllocator> CommandAllocator() {
			return g_CmdAllocator;
		}

		void AddEntity(Entity* e) {
			g_EntityResource = e;
		}

		Entity* Entity() {
			return g_EntityResource;
		}

		UINT Index() {
			return g_FrameResourceIndex;
		}

		UINT CBVDescriptorIndex() {
			return g_FrameResourceIndex;
		}

		void SetCBVDescriptorIndex(UINT i) {
			g_CBVDescriptorHeapIndex = i;
		}

		UINT64 Fence() {
			return g_CurrentFence;
		}

		void UpdateFence(UINT64 fence) {
			g_CurrentFence = fence;
		}

		void Update(ComPtr<ID3D12GraphicsCommandList> cmdList) {
			// This is called once per frame for each dynamic entity
			if(g_EntityResource) {
				g_EntityResource->UpdateBuffers();
			}
		}
	};
}