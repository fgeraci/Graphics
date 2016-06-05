#pragma once
#include "pch.h"

using namespace Microsoft::WRL;

namespace Application {
	
	ref class Gfx sealed {
	
	protected:

		property bool UseGPU{
			bool get() {
				return g_UseGPU;
			}
		}

	private:

		bool g_UseGPU;
		DXGI_MODE_DESC* g_PrimaryOutput;

		ComPtr<ID3D12Debug> g_DebugController;
		ComPtr<IDXGIFactory4> g_DxgiFactory;
		ComPtr<ID3D12Device> g_Device;

		void InitializeDebugging();
		void InitializeDirect3d();

		// Utility
		IDXGIAdapter* getGPU();
		void getPrimaryOutputProperties();
	};

}
