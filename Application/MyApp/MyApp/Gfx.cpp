#include "pch.h"
#include "Gfx.h"

using namespace Windows::UI::Core;
using namespace Application;
using namespace DX;

Gfx::Gfx() {
	LOGMESSAGE(L"Initializing Gfx\n");
	InitializeDebugging();
	InitializeDirect3D();
}

void Gfx::InitializeDebugging() {
#if defined (_DEBUG)
	DX::ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&g_DebugController)));
	g_DebugController->EnableDebugLayer();
	g_DebugEnabled = true;
	LOGMESSAGE(L"Debugging Initialized\n");
#endif
}

void Gfx::InitializeDirect3D() {
	
	LOGMESSAGE(L"1. Create Factory\n");
	// we could also use GetAddressOf to obtain the address of the interface pointer
	ThrowIfFailed(CreateDXGIFactory2(
		0, 
		IID_PPV_ARGS(&g_DxgiFactory)
	));

	LOGMESSAGE(L"2. Create Device\n");
	try {
		ThrowIfFailed(D3D12CreateDevice(
			(g_UseGPU ? GetGPU() : nullptr),
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

	LOGMESSAGE(L"3. Create Fence\n");
	ThrowIfFailed(g_Device->CreateFence(g_CurrentFence,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&g_Fence)));

	LOGMESSAGE(L"4. Obtain Descriptors sizes\n");
		// Render Targets
	g_RtvDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		// Depth/Stencil
	g_DsvDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		// Buffers, Shaders,Unordered Resources
	g_SrvDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	LOGMESSAGE(L"5. Check MSAA quality support levels and output information\n");
	CheckAntialisngSupport();
	GetClientOutputProperties();

	LOGMESSAGE(L"6. Create Command Queue, Allocator and List\n");
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(g_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_CommandQueue)));
	ThrowIfFailed(g_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(g_CommandAllocator.GetAddressOf())));
	ThrowIfFailed(g_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_CommandAllocator.Get(), 
		nullptr, // pipeline state object <- should any pre-rendering is needed, we would need a D3D12Pipeline object
		IID_PPV_ARGS(g_CommandList.GetAddressOf())));
	// start out closed
	ThrowIfFailed(g_CommandList->Close());

	LOGMESSAGE(L"7. Initialize Swap Chain\n");
		// first we describe the swap chain
	CreateSwapChain();
	
	LOGMESSAGE(L"8. Create descriptor heps\n");
	CreateDescriptorHeaps();

	LOGMESSAGE(L"9. Create render target views, set swap chain buffers size, set depth stencil and flush\n");
	ResizeBuffers();
	
}

void Gfx::Draw() {

	ThrowIfFailed(g_CommandAllocator->Reset());
	ThrowIfFailed(g_CommandList->Reset(g_CommandAllocator.Get(), nullptr));
	
	g_CommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	g_CommandList->RSSetViewports(1, &g_ViewPort);
	g_CommandList->RSSetScissorRects(1, &g_ScissorsRectangle);
	g_CommandList->ClearRenderTargetView(GetCurrentRtv(), DirectX::Colors::LightSteelBlue, 0, nullptr);
	g_CommandList->ClearDepthStencilView(GetDst(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	g_CommandList->OMSetRenderTargets(1, &GetCurrentRtv(), true, &GetDst());
	g_CommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	CloseCommandList();
	
	ThrowIfFailed(g_SwapChain->Present(0, 0));
	g_CurrentBackbuffer = (g_CurrentBackbuffer + 1) % g_SwapChainBuffersCount;

	FlushGPUCommandsQueue();
}

/* Utility */

IDXGIAdapter1* Gfx::GetGPU() {
	if (g_DxgiFactory == nullptr) return nullptr;
	ComPtr<IDXGIAdapter1> adapter = nullptr;
	ComPtr<IDXGIAdapter1> gpuAdapter = nullptr;
	short i = 0;
	UINT maxMemFound = 0;
	while (g_DxgiFactory->EnumAdapters1(i, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		if (desc.DedicatedVideoMemory > maxMemFound) {
			gpuAdapter = adapter.Detach();
			maxMemFound = desc.DedicatedVideoMemory;
		}
		i++;
	}
	return gpuAdapter.Get();
}

void Gfx::CreateSwapChain() {
	g_SwapChain.Reset();
	DXGI_SWAP_CHAIN_DESC1 swpDesc;
	swpDesc = { 0 };											// initialize all members to 0
	swpDesc.Width = 0;											// autoset
	swpDesc.Height = 0;
	swpDesc.Scaling = DXGI_SCALING_NONE;
	swpDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// buffers usage
	swpDesc.BufferCount = g_SwapChainBuffersCount;				// use double buffering (draw and swap)
	swpDesc.Format = g_BackbufferFormat;						// reg green blue and alpha
	swpDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;			// what will flipping cause?
	swpDesc.SampleDesc.Count = m_MsaaEnabled ?
		g_CurrentMSAaLevel : 1;									// if any, make it 4x, otherwise 1
	swpDesc.SampleDesc.Quality =								// set the quality levels
		m_MsaaEnabled ? (g_MsaaQualityLevels - 1) : 0;
	swpDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// create the swap chain
	ThrowIfFailed(g_DxgiFactory->CreateSwapChainForCoreWindow(
		g_CommandQueue.Get(), reinterpret_cast<IUnknown*>(CoreWindow::GetForCurrentThread()), &swpDesc, nullptr, &g_SwapChain));
}

void Gfx::GetClientOutputProperties() {
	// enumerates all adapter, outputs and modes for them
	ComPtr<IDXGIAdapter> adapter;
	UINT u = 0;
	while(g_DxgiFactory->EnumAdapters(u,&adapter) != DXGI_ERROR_NOT_FOUND) {
		
		ComPtr<IDXGIOutput> out = nullptr;
		std::vector<DXGI_MODE_DESC>output(1);
		UINT i = 0;
		DXGI_ADAPTER_DESC adapterDesc;
		adapter->GetDesc(&adapterDesc);

		std::wstring a = L"Adapter: ";
		a += adapterDesc.Description;
		a += L"\n";
		LOGMESSAGE(a.c_str());

		// get primary monitor description
		while (adapter->EnumOutputs(i, &out) != DXGI_ERROR_NOT_FOUND) {
		
			// Identify monitros for adapter
			UINT count = 0, flags = 0;
			DXGI_OUTPUT_DESC outDesc;
			out->GetDesc(&outDesc);
			std::wstring s = L"Output: ";
			s += outDesc.DeviceName; 
			s += L"\n";
			LOGMESSAGE(s.c_str());
		
			// populate mode descriptions lists
			out->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, flags, &count, nullptr);
			std::vector<DXGI_MODE_DESC>modes(count);
			out->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, flags, &count, &modes[0]);
			// we can use the DXGI_MODE_DESC here
			i++;
			out.Reset();
		}
	u++;
	adapter.Reset();
	}

	g_ClientWidth = g_PrimaryOutput.Width == 0 ?
		CoreWindow::GetForCurrentThread()->Bounds.Width : g_PrimaryOutput.Width;
	g_ClientHeight = g_PrimaryOutput.Height == 0 ?
		CoreWindow::GetForCurrentThread()->Bounds.Height : g_PrimaryOutput.Height;
}

void Gfx::CheckAntialisngSupport() { 
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAA;
	MSAA.Format = g_BackbufferFormat;
	MSAA.SampleCount = 4; // at least X4 should be supported
	MSAA.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MSAA.NumQualityLevels = 0;
	ThrowIfFailed(g_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MSAA, sizeof(MSAA)));
	g_MsaaQualityLevels = MSAA.NumQualityLevels; 
	// this is a hack, the pointer of the literal string is always true, if the first condition is false, it will print it.
	assert(g_MsaaQualityLevels > 0 && "Multisampling not supported");
}

void Gfx::CreateDescriptorHeaps() {
	
	// render target descriptor
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = { };
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = g_SwapChainBuffersCount;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	// depth stensil descriptor
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1; // we need only 1 depth buffer
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	// create heaps
	ThrowIfFailed(g_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_RtvDescHeap)));
	ThrowIfFailed(g_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&g_DsvDescHeap)));
}

void Gfx::CreateDepthStencilBuffer() {

	// describe the texture (resource) we will use as depth/stencil buffer
	CD3DX12_RESOURCE_DESC dsBufferDesc;
	dsBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsBufferDesc.Alignment = 0; // memory alignment, default 4MB for Multisampling - 64kb verything else
	dsBufferDesc.Width = g_ClientWidth;
	dsBufferDesc.Height = g_ClientHeight;
	dsBufferDesc.DepthOrArraySize = 1;
	dsBufferDesc.MipLevels = 1;
	dsBufferDesc.Format = g_DepthStencilFormat;
	dsBufferDesc.SampleDesc.Count = m_MsaaEnabled ? g_CurrentMSAaLevel : 1;
	dsBufferDesc.SampleDesc.Quality =
		m_MsaaEnabled ? (g_MsaaQualityLevels - 1) : 0;
	dsBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	dsBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	// make it a Depth/Stencil buffer

	// create an optimized clear value
	D3D12_CLEAR_VALUE optimizedClear;
	optimizedClear.Format = g_DepthStencilFormat;
	optimizedClear.DepthStencil.Depth = 1.0f;
	optimizedClear.DepthStencil.Stencil = 0;

	// create committed resource (actual buffer texture)
	ThrowIfFailed(g_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),		// CPU read/write privileges - Default is GPU-only priv
		D3D12_HEAP_FLAG_NONE,
		&dsBufferDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optimizedClear,
		IID_PPV_ARGS(&g_DepthStencilBuffer)));

	// create the depth stencil view and attach it to the descriptor
	g_Device->CreateDepthStencilView(
		g_DepthStencilBuffer.Get(), 
		nullptr, 
		GetDst());
	
	// transition resource state to be used as buffer (enables resource)
	g_CommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(g_DepthStencilBuffer.Get(), 
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		
}

void Gfx::FlushGPUCommandsQueue() {
	g_CurrentFence++;
	ThrowIfFailed(g_CommandQueue->Signal(g_Fence.Get(), g_CurrentFence));
	if (g_Fence->GetCompletedValue() < g_CurrentFence) {
		HANDLE signalHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(g_Fence->SetEventOnCompletion(g_CurrentFence, signalHandle));
		WaitForSingleObject(signalHandle, INFINITE);
		CloseHandle(signalHandle);
	}
}

ID3D12Resource* Gfx::GetCurrentBackBuffer() {
	return g_Backbuffers[g_CurrentBackbuffer].Get();
}

void Gfx::CloseCommandList() {
	ThrowIfFailed(g_CommandList->Close());
	ID3D12CommandList* lists[] = { g_CommandList.Get() };
	g_CommandQueue->ExecuteCommandLists(_countof(lists), lists);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Gfx::GetCurrentRtv() {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		g_RtvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		g_CurrentBackbuffer,
		g_RtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Gfx::GetDst() {
	return g_DsvDescHeap->GetCPUDescriptorHandleForHeapStart();
}

void Gfx::ResizeBuffers() {
	
	LOGMESSAGE(L"Flush the queue and reset buffers\n");
	FlushGPUCommandsQueue();

	ThrowIfFailed(g_CommandList->Reset(g_CommandAllocator.Get(), nullptr));

	for (int i = 0; i < g_SwapChainBuffersCount; ++i) {
		g_Backbuffers[i].Reset();
	}
	g_DepthStencilBuffer.Reset();

	LOGMESSAGE(L"Set the new buffers' size\n");
	ThrowIfFailed(g_SwapChain->ResizeBuffers(g_SwapChainBuffersCount,
		g_ClientWidth,
		g_ClientHeight,
		g_BackbufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	g_CurrentBackbuffer = 0;

	LOGMESSAGE(L"Create render target views\n");
	CD3DX12_CPU_DESCRIPTOR_HANDLE tmpHeapHandle(g_RtvDescHeap->GetCPUDescriptorHandleForHeapStart()) ;
	for (int i = 0; i < g_SwapChainBuffersCount; ++i) {
		ThrowIfFailed(g_SwapChain->GetBuffer(i, IID_PPV_ARGS(&g_Backbuffers[i])));
		// set the rtv descriptor in the ith buffer
		g_Device->CreateRenderTargetView(g_Backbuffers[i].Get(), nullptr, tmpHeapHandle);
		// move the heap forward
		tmpHeapHandle.Offset(1, g_RtvDescriptorSize);
	}

	LOGMESSAGE(L"Reset depth/stencil buffer\n");
	CreateDepthStencilBuffer();


	CloseCommandList();
	FlushGPUCommandsQueue();

	LOGMESSAGE(L"Set the Viewport\n");
	// describe the viewport
	g_ViewPort = {};
	g_ViewPort.MaxDepth = 1;
	g_ViewPort.MinDepth = 0;
	g_ViewPort.Height = static_cast<float>(g_ClientHeight);
	g_ViewPort.Width = static_cast<float>(g_ClientWidth);
	g_ViewPort.TopLeftX = g_ViewPort.TopLeftY = 0.0f;

	LOGMESSAGE(L"Define Scissors Rectanlges if needed\n");
	g_ScissorsRectangle = { 0,0,static_cast<long>(g_ClientWidth / 2),static_cast<long>(g_ClientHeight / 2) };
}

void Gfx::Terminate() {
	LOGMESSAGE(L"Finalizing graphics ... \n");
	assert(g_Device);
	FlushGPUCommandsQueue();
	g_Device.Reset();
}