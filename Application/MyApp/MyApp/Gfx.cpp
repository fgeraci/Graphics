#include "pch.h"
#include "Gfx.h"

using namespace Application;
using namespace DX;

void Gfx::InitializeDebugging() {
#if defined(DEBUG) || defined (_DEBUG)
	DX::ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&g_DebugController)));
	g_DebugController->EnableDebugLayer();
#endif
}

void Gfx::InitializeDirect3d() {
	
	// we could also use GetAddressOf to obtain the address of the interface pointer
	ThrowIfFailed(CreateDXGIFactory2(
		0, 
		IID_PPV_ARGS(&g_DxgiFactory)
	));

	try {
		ThrowIfFailed(D3D12CreateDevice(
			g_UseGPU ? Gfx::getGPU() : nullptr,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&g_Device)
		));
	} catch (HRESULT e) {
		// fallback to WARP - Windows Advanced Rasterizer Platform = Software "GPU"
		ComPtr<IDXGIAdapter> warpAdapter;
		// obtain warp
		ThrowIfFailed(g_DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
		// create the device
		ThrowIfFailed(D3D12CreateDevice(
			warpAdapter.Get(), 
			D3D_FEATURE_LEVEL_11_0, 
			IID_PPV_ARGS(&g_Device)
		));
	}

}

/* Utility */

IDXGIAdapter* Gfx::getGPU() {
	if (g_DxgiFactory == nullptr) return nullptr;
	IDXGIAdapter* adapter = nullptr;
	IDXGIAdapter* gpuAdapter = nullptr;
	short i = 0;
	long maxMemFound = -1;
	while (g_DxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);
		if (desc.DedicatedVideoMemory > maxMemFound) {
			gpuAdapter = adapter;
			maxMemFound = desc.DedicatedVideoMemory;
		}
	}
	return gpuAdapter;
}

void Gfx::getPrimaryOutputProperties() {
	IDXGIAdapter* adapter;
	ThrowIfFailed(g_DxgiFactory->EnumAdapterByLuid(g_Device->GetAdapterLuid(), IID_PPV_ARGS(&adapter)));
	IDXGIOutput* out;
	g_PrimaryOutput = nullptr;

	// get primary monitor description
	if (adapter->EnumOutputs(0, &out) != DXGI_ERROR_NOT_FOUND) {
		UINT count = 0;
		out->GetDisplayModeList(DXGI_FORMAT_R16G16B16A16_FLOAT, 0, &count, g_PrimaryOutput);
	}
}