#pragma once

#include "pch.h"

namespace Application {
	
	class FrameResources {
	
	private:
		
		ComPtr<ID3D12CommandAllocator> g_CmdAllocator;
		
		// Beginning Fence
		UINT64 g_CurrentFence;
		
		// Resources
		UploadBuffer<ObjectConstantData>* g_ConstantBuffer;
		Entity* g_EntityResource;

	public:

		FrameResources(ComPtr<ID3D12Device> device, UINT64 fence) {
			g_CurrentFence = fence;
			DX::ThrowIfFailed(device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_CmdAllocator))
			);
		}

		FrameResources(ComPtr<ID3D12Device> device, UINT64 fence, Entity* e) : FrameResources(device,fence) {
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

		UINT64 Fence() {
			return g_CurrentFence;
		}

		void UpdateFence(UINT64 fence) {
			g_CurrentFence = fence;
		}

		void Update(ComPtr<ID3D12GraphicsCommandList> cmdList) {
			if(g_EntityResource) {
				/* Test */
				if (g_EntityResource->Name() == L"Cube") {
					g_EntityResource->Transform(TRANSFORMATION_TYPE::ROTATE, DIRECTION::WORLD_LEFT);
					g_EntityResource->Transform(TRANSFORMATION_TYPE::ROTATE, DIRECTION::LOCAL_RIGHT,5.0f);
				}
				/*     */
				g_EntityResource->UpdateVertexBuffer();
			}
		}
	};
}