#pragma once

#include "pch.h"

using namespace Microsoft::WRL;

namespace Application {
	
	/*
	*	Buffer - Generic wrapper class for an ID3DResource buffer, it could be defined as constant.
	*/
	template<typename T>
	class UploadBuffer {

	public:

		UINT UnitSize() {
			return g_UnitSize;
		}

		UINT TotalSize() {
			return g_TotalSize;
		}

		bool IsConstant() {
			return g_IsConstantBuffer;
		}

		UploadBuffer(ComPtr<ID3D12Device> device,
			UINT elementsCount, bool constantBuffer = false)
			: g_IsConstantBuffer(constantBuffer) {

			g_UnitSize = sizeof(T);

			if (g_IsConstantBuffer) {
				g_TotalSize = ((g_UnitSize * elementsCount) + 255) & ~255;
			}
			else g_TotalSize = g_UnitSize * elementsCount;

			DX::ThrowIfFailed(device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),		// type of heap this buffer will go to
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(g_TotalSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,						// initial resource barrier state
				nullptr,
				IID_PPV_ARGS(&g_Buffer)
			));

			DX::ThrowIfFailed(g_Buffer->Map(0, nullptr, reinterpret_cast<void**>(&g_MappedData)));

			// Do not write while the GPU is using it
		}

		UploadBuffer(const UploadBuffer& rhs) = delete;
		UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
		~UploadBuffer() {
			if (g_Buffer != nullptr) {
				g_Buffer->Unmap(0, nullptr);
			}
			g_MappedData = nullptr;
		}

		void SetCBVDescriptor(D3D12_CONSTANT_BUFFER_VIEW_DESC d) {
			g_CBVDesc = d;
		}

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDescriptor() {
			return g_CBVDesc;
		}

		ID3D12Resource* GetResource() const {
			return g_Buffer.Get();
		}

		void WriteToBuffer(const T& data, UINT position) {
			memcpy(&g_MappedData[position*g_UnitSize], &data, g_TotalSize);
		}

	private:

		UINT g_TotalSize = 0;
		UINT g_UnitSize = 0;
		bool g_IsConstantBuffer;
		D3D12_CONSTANT_BUFFER_VIEW_DESC g_CBVDesc;
		ComPtr<ID3D12Resource> g_Buffer = nullptr;
		BYTE* g_MappedData = nullptr;

	};
}
