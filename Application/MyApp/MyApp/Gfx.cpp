#include "pch.h"

using namespace Windows::UI::Core;
using namespace Application;
using namespace DX;
using namespace Application::Enums::Transformation;
using namespace Windows::Graphics::Display;

#pragma region Constructor

Gfx::Gfx() {
	
	LOGMESSAGE(L"Gfx --> Initializing\n");
	InitializeDebugging();
	InitializeDirect3D();
	InitializeMainCamera();
	InitializePipeline();

	
	g_WorldEntity = std::make_unique<WorldEntity>();
	
	CloseGlobalCommandList();
	FlushGPUCommandsQueueAll();
	
	LOGMESSAGE(L"Gfx --> Initialization completed\n");
}

#pragma endregion Constructor

#pragma region Draw

void Gfx::Draw() {

	ComPtr<ID3D12CommandAllocator> curAllocator;
	FrameResources* fr = nullptr;
	if(g_FrameResources.size() > 0) {
		/* Handle current FrameResources */
		g_CurrentFrameResources = (g_CurrentFrameResources + 1) % g_FrameResources.size();
		fr = g_FrameResources[g_CurrentFrameResources];
		curAllocator = fr->CommandAllocator();
		g_CurrentFence = g_Fence->GetCompletedValue();
		if(g_CurrentFence < fr->Fence()) {
			FlushGPUCommandsQueueForFence(fr->Fence());
		}
		g_CurrentFence = g_Fence->GetCompletedValue() + 1;
		fr->UpdateFence(g_CurrentFence);
	} else {
		curAllocator = g_CommandAllocator;
	}

	// proceeed when freed
	ThrowIfFailed(curAllocator->Reset());

	// We can define in this call which pipeline state to use
	ThrowIfFailed(g_CommandList->Reset(curAllocator.Get(), g_PipelineStateTriangle.Get()));

	g_CommandList->RSSetViewports(1, &g_ViewPort);
	g_CommandList->RSSetScissorRects(1, &g_ScissorsRectangle);
	g_CommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	g_CommandList->ClearRenderTargetView(GetCurrentRtv(), g_BackBufferColor, 0, nullptr);
	g_CommandList->ClearDepthStencilView(GetDsv(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	g_CommandList->OMSetRenderTargets(1, &GetCurrentRtv(), true, &GetDsv());
	
	/*   Send drawing commands here	*/
	
	// Update view every frame
	UpdateCamera();

	// Update frame's constant data
	UpdateFrameConstantData();

	

	// Draw all of them
	int allEntities = static_cast<int>(g_FrameResources.size());
	for(int i = 0; i < allEntities; ++i) {
		
		FrameResources* currFr = g_FrameResources[i];
		currFr->Update(g_CommandList);
		Entity* e = g_FrameResources[i]->Entity();
		ComPtr<ID3D12DescriptorHeap> heap = g_CBVDescriptorsHeaps[currFr->CBVDescriptorIndex()];
		
		// Set the buffer in the root signature descriptors
		ComPtr<ID3D12DescriptorHeap> descHeaps[] = { heap.Get() };
		g_CommandList->SetDescriptorHeaps(_countof(descHeaps), descHeaps->GetAddressOf());

		g_CommandList->SetGraphicsRootSignature(g_RootSignature.Get());

		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvDescHandle(heap->GetGPUDescriptorHandleForHeapStart());
		g_CommandList->SetGraphicsRootDescriptorTable(0, cbvDescHandle);

		if(e) {
			if (e->IsEnabled() && e->IsDrawable()) {

				std::vector<D3D12_VERTEX_BUFFER_VIEW> vbvlist = { e->VBView() };
				std::vector<D3D12_INDEX_BUFFER_VIEW> ibvlist = { e->IBView() };

				g_CommandList->IASetVertexBuffers(
					e->StartVertexLocation() + (static_cast<int>(vbvlist.size()) - 1),			// start assembler input slot (0-15)
					static_cast<int>(vbvlist.size()),											// we will bind to input slots (startSlots + (numBuffers-1))
					vbvlist.data());
				g_CommandList->IASetIndexBuffer(ibvlist.data());
				g_CommandList->IASetPrimitiveTopology(e->Topology());

				// select the proper pilepile based on the required properties- this is NOT ideal per draw call, but will do for now.
				g_CommandList->SetPipelineState(
					e->Topology() == D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE ? g_PipelineStateLine.Get() : g_PipelineStateTriangle.Get());

				// Select the proper subresource in the heap
				cbvDescHandle.Offset(1, g_CbvSrvDescriptorSize);
				g_CommandList->SetGraphicsRootDescriptorTable(1, cbvDescHandle);

				g_CommandList->DrawIndexedInstanced(e->IndicesNumber(), 1, 0, 0, 0);
			}
		}
	}

	/*   End of drawwing commands	*/

	g_CommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	CloseGlobalCommandList();

	ThrowIfFailed(g_SwapChain->Present(0, 0));
	g_CurrentBackbuffer = (g_CurrentBackbuffer + 1) % g_SwapChainBuffersCount;

	g_CommandQueue->Signal(g_Fence.Get(), g_CurrentFence);

}

#pragma endregion Draw

#pragma region Initialization

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
			(g_UseGPU ? GetGPU().Get() : nullptr),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&g_Device)
		));
	} catch (...) { // catch all
		
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
	g_CbvSrvDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

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

	// Secondary CommandList only for entities creation
	InitializeEntityCommandList();

	LOGMESSAGE(L"7. Initialize Swap Chain\n");
		// first we describe the swap chain
	CreateSwapChain();
	
	LOGMESSAGE(L"8. Create descriptor heps\n");
	CreateDescriptorHeaps();

	LOGMESSAGE(L"9. Create render target views, set swap chain buffers size, set depth stencil and flush\n");
	ResizeBuffers();
	
}

void Gfx::InitializeMainCamera() {
	g_MainCamera = std::make_unique<Camera>(true, g_MainCameraInitialPosition);
	g_MainCamera->InitializeUploadBuffer(g_Device);
	g_MainPerFrameBuffer = g_MainCamera->GetUploadBuffer();
}

void Gfx::InitializeEntityCommandList() {
	ThrowIfFailed(g_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(g_EntityCommandList.commandAllocator.GetAddressOf())));
	ThrowIfFailed(g_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_EntityCommandList.commandAllocator.Get(),
		nullptr, // pipeline state object <- should any pre-rendering is needed, we would need a D3D12Pipeline object
		IID_PPV_ARGS(g_EntityCommandList.commandList.GetAddressOf())));
	CloseEntitiesCommandLists();
}

#pragma endregion Initialization

#pragma region Accessors / Mutators

Entity* Gfx::MainCamera() {
	return static_cast<Entity*>(g_MainCamera.get());
}

#pragma endregion Accessors / Mutators

#pragma region Utility

/* Utility */

ComPtr<IDXGIAdapter1> Gfx::GetGPU() {
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
			maxMemFound = static_cast<int>(desc.DedicatedVideoMemory);
		}
		i++;
	}
	adapter = nullptr;
	return gpuAdapter;
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

	// Determine scaling
	g_ClientScaling = static_cast<float>(DisplayInformation::GetForCurrentView()->ResolutionScale) / 100.0f;
	g_ClientWidth = (UINT)(g_ClientScaling * (g_PrimaryOutput.Width == 0 ?
		static_cast<UINT>(CoreWindow::GetForCurrentThread()->Bounds.Width) : static_cast<UINT>(g_PrimaryOutput.Width)));
	g_ClientHeight = (UINT)(g_ClientScaling * (g_PrimaryOutput.Height == 0 ?
		static_cast<UINT>(CoreWindow::GetForCurrentThread()->Bounds.Height) : static_cast<UINT>(g_PrimaryOutput.Height)));
	
}

void Gfx::CheckAntialisngSupport() { 
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAA;
	MSAA.Format = g_BackbufferFormat;
	MSAA.SampleCount = 8; // at least X4 should be supported
	MSAA.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MSAA.NumQualityLevels = 4;
	ThrowIfFailed(g_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MSAA, sizeof(MSAA)));
	g_MsaaQualityLevels = MSAA.NumQualityLevels; 
	// this is a hack, the pointer of the literal string is always true, if the first condition is false, it will print it.
	assert(g_MsaaQualityLevels > 0 && "Multisampling not supported");
}

void Gfx::CreateDescriptorHeaps() {
	
	// render target heap descriptor
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

	// constant buffers descriptors
	// this can be done when initializing the pipeline
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// shader need access to it!
	// One for the frame-wise resources, another one for each object to set per draw
	cbvHeapDesc.NumDescriptors = g_ConstantBuffers;
	cbvHeapDesc.NodeMask = 0;


	// create heaps
	ThrowIfFailed(g_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_RtvDescHeap)));
	ThrowIfFailed(g_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&g_DsvDescHeap)));
	ThrowIfFailed(g_Device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&g_CbvDescHeap)));
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
		GetDsv());
	
	// transition resource state to be used as buffer (enables resource)
	g_CommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(g_DepthStencilBuffer.Get(), 
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		
}

void Gfx::FlushGPUCommandsQueueAll() {
	g_CurrentFence++;
	ThrowIfFailed(g_CommandQueue->Signal(g_Fence.Get(), g_CurrentFence));
	if (g_Fence->GetCompletedValue() < g_CurrentFence) {
		HANDLE signalHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(g_Fence->SetEventOnCompletion(g_CurrentFence, signalHandle));
		WaitForSingleObject(signalHandle, INFINITE);
		CloseHandle(signalHandle);
	}
}

void Gfx::FlushGPUCommandsQueueForFence(UINT64 forFence) {
	HANDLE signalHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	ThrowIfFailed(g_Fence->SetEventOnCompletion(forFence, signalHandle));
	WaitForSingleObject(signalHandle, INFINITE);
	CloseHandle(signalHandle);
}

ID3D12Resource* Gfx::GetCurrentBackBuffer() {
	return g_Backbuffers[g_CurrentBackbuffer].Get();
}

void Gfx::CloseEntitiesCommandLists() {
	ThrowIfFailed(g_EntityCommandList.commandList->Close());
	ID3D12CommandList* lists[] = { g_EntityCommandList.commandList.Get() };
	g_CommandQueue->ExecuteCommandLists(1, lists);
}

void Gfx::CloseGlobalCommandList() {
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

D3D12_CPU_DESCRIPTOR_HANDLE Gfx::GetDsv() {
	return g_DsvDescHeap->GetCPUDescriptorHandleForHeapStart();
}

ComPtr<ID3D12Resource> Gfx::CreateDefaultBuffer(UINT byteSize, const void* data, ComPtr<ID3D12Resource>& uploadBuffer) {
	
	ComPtr<ID3D12Resource> newBuffer;
	
	// 1. create default buffer (GPU only access)
	// we use CD3DX12 wrapper calss which provcides utility methods for creating and describing plain common buffer reosurces
	ThrowIfFailed(g_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),		// type of heap this buffer will go to
		D3D12_HEAP_FLAG_NONE,									// resource flags
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),				// descr struct of resource which includes size
		D3D12_RESOURCE_STATE_COMMON,							// initial resource barrier state
		nullptr,												// resource id, none
		IID_PPV_ARGS(newBuffer.GetAddressOf())					// out pointer
	));
	

	// 2. create upload heap buffer (cpu initialize for then copying to GPU)
	if (uploadBuffer == nullptr) {
		ThrowIfFailed(g_Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),		// type of heap this buffer will go to
			D3D12_HEAP_FLAG_NONE,									
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),				
			D3D12_RESOURCE_STATE_GENERIC_READ,						// initial resource barrier state
			nullptr,												
			IID_PPV_ARGS(uploadBuffer.GetAddressOf())				
		));
	}

	// 3. Describe the data we will write to the buffers
	D3D12_SUBRESOURCE_DATA srd = {};
	srd.pData = data;
	srd.RowPitch = byteSize;
	srd.SlicePitch = srd.RowPitch;

	// 4. Have the CPU to copy the Data to the GPUs buffer
		// Make the new default buffer to be a copy destination
	

	g_EntityCommandList.commandList->ResourceBarrier(
		1,&CD3DX12_RESOURCE_BARRIER::Transition(newBuffer.Get(),D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST)
	);
		// Schedule the work
	UpdateSubresources<1>(g_EntityCommandList.commandList.Get(), newBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &srd);
		// make the new default buffer to be a generic readable only
	g_EntityCommandList.commandList->ResourceBarrier(
		1, &CD3DX12_RESOURCE_BARRIER::Transition(newBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ)
	);

	// 5. Return the new buffer
	return newBuffer;
}

void Gfx::UpdateCamera() {

	// update camera entity's state
	if (g_MainCamera->IsDirty()) {
		// obtain matrices
		XMVECTOR position = g_MainCamera->Position();
		XMVECTOR target = g_MainCamera->Target();
		XMVECTOR up = Application::m_WorldUpVector;

		// compute projection matrix
		g_MainCamera->SetView(XMMatrixLookAtLH(position, target, up));
		g_MainCamera->SetProjection(XMMatrixPerspectiveFovLH(
			m_ViewAngleVertical*XM_PI, 
			static_cast<float>(g_ClientWidth) / g_ClientHeight, 
			m_DepthOfViewNearZ, m_DepthOfViewFarZ));
		g_MainCamera->SetViewProject(g_MainCamera->View() * g_MainCamera->Projection());
	}
}

void Gfx::UpdateFrameConstantData() {
	
	/* 
		Due to the fact DirectX is ROW-MAJOR (matrices in memroy),
		we need to transpose the matrices. This does not affect the
		underlying math.
	*/

	// update frame's CBV buffer
	FrameConstantData frameBuffer;
	// view
	XMStoreFloat4x4(&frameBuffer.View, g_MainCamera->View());
	XMStoreFloat4x4(&frameBuffer.InvView, 
		XMMatrixInverse(&XMMatrixDeterminant(g_MainCamera->View()),g_MainCamera->View()));					// the inverse is the transposed of the iverse
	// proj
	XMStoreFloat4x4(&frameBuffer.Proj, g_MainCamera->Projection());
	XMStoreFloat4x4(&frameBuffer.InvProj,
		XMMatrixInverse(&XMMatrixDeterminant(g_MainCamera->Projection()), g_MainCamera->Projection()));
	// viewProj
	XMStoreFloat4x4(&frameBuffer.ViewProj, XMMatrixTranspose(g_MainCamera->ViewProject()));
	XMStoreFloat4x4(&frameBuffer.InvViewProj,
		XMMatrixInverse(&XMMatrixDeterminant(g_MainCamera->ViewProject()), g_MainCamera->ViewProject()));
	// cbPadding
	frameBuffer.cbPerObjectPad1 = 0.0f;
	// Eye Position
	XMStoreFloat3(&frameBuffer.EyePosW,g_MainCamera->Position());

	frameBuffer.RTSize		= XMFLOAT2(static_cast<float>(g_ClientWidth), static_cast<float>(g_ClientHeight));
	frameBuffer.InvRTSize	= XMFLOAT2(1.0f / g_ClientWidth, 1.0f / g_ClientHeight);

	frameBuffer.NearZ		= m_DepthOfViewNearZ;
	frameBuffer.FarZ		= m_DepthOfViewFarZ;

	g_MainCamera->GetUploadBuffer()->WriteToBuffer(frameBuffer, 0);
}

void Gfx::UpdateGrid() {
	// if the grid is dynamic, update here - TODO
}

Entity* Gfx::AddPolygon(POLYGON_TYPE type) {
	return AddPolygon(type, false);
}

Entity* Gfx::AddPolygon(POLYGON_TYPE type, bool dyn) {
	
	// Check there are no commands pending for this allocator
	if (g_Fence->GetCompletedValue() < g_EntityCommandList.fence) {
		FlushGPUCommandsQueueForFence(g_EntityCommandList.fence);
	}

	g_CurrentFence = g_Fence->GetCompletedValue() + 1;
	ThrowIfFailed(g_EntityCommandList.commandAllocator->Reset());
	ThrowIfFailed(g_EntityCommandList.commandList->Reset(g_EntityCommandList.commandAllocator.Get(), nullptr));

	Polygon *p = nullptr;
	UINT totalVertices = 0, totalIndices = 0;

	switch (type) {
	case CUBE:
		p = new Cube(dyn);
		break;
	case GRID:
		g_Grid = std::make_unique<Grid>(dyn);
		p = g_Grid.get();
		break;
	case CYLINDER:
		p = new Cylinder(dyn);
		break;
	case ICOSAHEDRON:
		p = new Icosahedron(dyn);
		break;
	}
	// Create the Vertices buffer
		
	ComPtr<ID3D12Resource> tmpVUBuffer = nullptr;
	ComPtr<ID3D12Resource> tmpVBuffer = nullptr;

	p->InitConstantBuffer(g_Device);

	if(p->IsDynamic()) {
		p->InitializeUploadBuffer(g_Device);
		tmpVUBuffer = p->CPUVertexBuffer();
		tmpVBuffer = p->CPUVertexBuffer();
		p->UpdateBuffers();
	} else tmpVBuffer = CreateDefaultBuffer(p->VBBytesSize(), reinterpret_cast<void*>(p->Vertices().data()), tmpVUBuffer);
	
	totalVertices += p->VerticesNumber();

	// Create the VB descriptor
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.SizeInBytes = p->VBBytesSize();							// size of the buffer
	vbv.BufferLocation = tmpVBuffer->GetGPUVirtualAddress();	// location in GPU memory
	vbv.StrideInBytes = Polygon::StrideSize;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> list = { vbv };
	p->SetVBView(vbv);

	// Create the Indices buffer
	totalIndices += p->IndicesNumber();
	ComPtr<ID3D12Resource> tmpIUBuffer = nullptr;
	ComPtr<ID3D12Resource> tmpIBuffer = CreateDefaultBuffer(p->IBBytesSize(), reinterpret_cast<void*>(p->Indices().data()), tmpIUBuffer);
		
	// Create the IB descriptor
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.Format = Polygon::IndexFormat;
	ibv.SizeInBytes = p->IBBytesSize();
	ibv.BufferLocation = tmpIBuffer->GetGPUVirtualAddress();
	p->SetIBView(ibv);
		
	p->SetCPUVertexBuffer(tmpVUBuffer);
	p->SetCPUIndexBuffer(tmpIUBuffer);
	p->SetGPUVertexBuffer(tmpVBuffer);
	p->SetGPUIndexBuffer(tmpIBuffer);

	tmpVUBuffer = tmpVBuffer = tmpIUBuffer = tmpIBuffer = nullptr;

	// add it to the queue
	FrameResources* fr = new FrameResources(g_Device, 0, p, static_cast<int>(g_FrameResources.size()));
	g_FrameResources.push_back(fr);

	/* Set the Constant Buffer in the Descriptors Heap */
	int heapIndex = g_ConstantBuffers - 1;
	D3D12_GPU_VIRTUAL_ADDRESS address = p->ConstantBuffer()->GetResource()->GetGPUVirtualAddress();
	int bufferSize = p->ConstantBuffer()->TotalSize();

	// Initialize its CBV descriptor heap
	int index = AddCBVDescriptorHeap();
	fr->SetCBVDescriptorIndex(index);
	ComPtr<ID3D12DescriptorHeap> heap = g_CBVDescriptorsHeaps[index];
	AddFrameConstantBuffers(heap);

	D3D12_CONSTANT_BUFFER_VIEW_DESC d;
	d.SizeInBytes = bufferSize;
	d.BufferLocation = address;

	// offset the first per frame descriptor
	CD3DX12_CPU_DESCRIPTOR_HANDLE h(heap->GetCPUDescriptorHandleForHeapStart(), 1, g_CbvSrvDescriptorSize);
	g_Device->CreateConstantBufferView(&d, h);

	// Close the entities queue
	CloseEntitiesCommandLists();

	// Do not move forward without creating resources, might cause misuse of buffers
	g_CurrentFence = g_Fence->GetCompletedValue() + 1;
	g_CommandQueue->Signal(g_Fence.Get(), g_CurrentFence);
	FlushGPUCommandsQueueForFence(g_CurrentFence);

	return p;
}

ComPtr<ID3DBlob> Gfx::LoadFileBlob(const std::wstring& fileName) {
	
	// read file and get size
	std::ifstream file(fileName, std::ios::binary);
	file.seekg(0, std::ios_base::end);
	std::ifstream::pos_type size = (int) file.tellg();
	file.seekg(0, std::ios_base::beg);

	// set up blob
	ComPtr<ID3DBlob> fileByteStream;
	ThrowIfFailed(D3DCreateBlob(static_cast<UINT>(size), fileByteStream.GetAddressOf()));
	
	// fill up blob
	file.read((char*)fileByteStream->GetBufferPointer(), size);
	file.close();
	return fileByteStream;
}

void Gfx::CreateInputLayout() {
	g_InputLayout = {
		{
			"POSITION",										// sematic name
			0,												// semantic index to add to name (in case there are more semantics with same name)
			DXGI_FORMAT_R32G32B32_FLOAT,					// dxgi format type
			0,												// Input-assembler identifier (0-15)
			0,												// offset in the struct
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		// this is per vertex struct
			0												// How many times to draw it before advancing - 0 means per_vertex
		},
		{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA ,0 }
	};
}

void Gfx::AddFrameConstantBuffers(ComPtr<ID3D12DescriptorHeap> heap) {
	
	// this address needs to be offset if we want to point forward in the heap
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = g_MainPerFrameBuffer->GetResource()->GetGPUVirtualAddress();
	
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvd = {};
	cbvd.BufferLocation = cbAddress;
	cbvd.SizeInBytes = g_MainPerFrameBuffer->TotalSize();
	
	// no offset in the heap
	g_Device->CreateConstantBufferView(&cbvd,
		heap->GetCPUDescriptorHandleForHeapStart());
}

int Gfx::AddCBVDescriptorHeap() {
	
	ComPtr<ID3D12DescriptorHeap> heap = nullptr;
	
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeaDesc;
	cbvHeaDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeaDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeaDesc.NumDescriptors = g_CbvSrvDescriptorSize;
	cbvHeaDesc.NodeMask = 0;
	g_Device->CreateDescriptorHeap(&cbvHeaDesc, IID_PPV_ARGS(&heap));
	
	g_CBVDescriptorsHeaps.push_back(heap);

	// Set per frame (permanent) descriptors in the heap
	CD3DX12_CPU_DESCRIPTOR_HANDLE h(heap->GetCPUDescriptorHandleForHeapStart());

	return static_cast<int>(g_CBVDescriptorsHeaps.size() - 1);
}

void Gfx::CreatePSO(D3D12_PRIMITIVE_TOPOLOGY_TYPE type, ComPtr<ID3D12PipelineState>& state) {
	// get resources
	ComPtr<ID3DBlob> vertexShader = LoadFileBlob(L"VertexShader.cso");
	ComPtr<ID3DBlob> pixelShader = LoadFileBlob(L"PixelShader.cso");
	// build the pipeline state
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineState = {};
	ZeroMemory(&pipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	pipelineState.InputLayout = { g_InputLayout.data(), (UINT)g_InputLayout.size() };
	pipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineState.pRootSignature = g_RootSignature.Get();
	pipelineState.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
	pipelineState.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	pipelineState.PrimitiveTopologyType = type;
	pipelineState.RTVFormats[0] = g_BackbufferFormat;
	pipelineState.NumRenderTargets = 1;
	pipelineState.DSVFormat = g_DepthStencilFormat;
	pipelineState.SampleDesc.Count = m_MsaaEnabled ?
		g_CurrentMSAaLevel : 1;
	pipelineState.SampleDesc.Quality = m_MsaaEnabled ? (g_MsaaQualityLevels - 1) : 0;
	pipelineState.SampleMask = UINT_MAX;
	// create the PSO
	g_Device->CreateGraphicsPipelineState(&pipelineState, IID_PPV_ARGS(&state));
}

void Gfx::CreateRootSignature() {

	LOGMESSAGE(L"\t\tCreate the Root Signature to register the Constant Buffer to the Pipeline registers\n");
	
	// Create the Root Parameter and the Descriptor Table, bind the Descriptor table to the parameter
	CD3DX12_ROOT_PARAMETER rootParams[2];		// 1 single parameter for the Root Signature
	
	// Create descriptor tables
	CD3DX12_DESCRIPTOR_RANGE descriptorTablePerFrame;
	CD3DX12_DESCRIPTOR_RANGE descriptorTablePerObject;

	descriptorTablePerFrame.Init(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV,		// Type of element to be described in the table
		1,										// how many
		0										// to which register (b0)
	);

	descriptorTablePerObject.Init(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV,		// Type of element to be described in the table
		1,										// how many
		1										// to which register (b1)
	);

	rootParams[0].InitAsDescriptorTable(
		1,										// Number of tables
		&descriptorTablePerFrame				// Head of array of tables
	);											// Default flag --> D3D12_SHADER_VISIBILITY_ALL

	rootParams[1].InitAsDescriptorTable(
		1,										// Number of tables
		&descriptorTablePerObject				// Head of array of tables
	);

	// Describe the Root Signature
	CD3DX12_ROOT_SIGNATURE_DESC rootSignDesc(
		_countof(rootParams),					// Number of parameters
		rootParams,								// The actual parameters array
		0,										// static samplers (???)				 
		nullptr,								// static sampler description
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT // Flag to allow access
	);

	// Serialize and create the Root Signature
	ComPtr<ID3DBlob> serializedRootSign = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSign.GetAddressOf(), errorBlob.GetAddressOf()));
	if (errorBlob != nullptr) {
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(g_Device->CreateRootSignature(
		0, 
		serializedRootSign->GetBufferPointer(),
		serializedRootSign->GetBufferSize(),
		IID_PPV_ARGS(&g_RootSignature)));
}

#pragma endregion Utility

#pragma region Reize

void Gfx::ResizeBuffers() {
	LOGMESSAGE(L"Resizing buffers ... \n");
	LOGMESSAGE(L"\tFlush the queue and reset buffers\n");
	FlushGPUCommandsQueueAll();

	ThrowIfFailed(g_CommandList->Reset(g_CommandAllocator.Get(), nullptr));

	for (int i = 0; i < g_SwapChainBuffersCount; ++i) {
		g_Backbuffers[i].Reset();
	}
	g_DepthStencilBuffer.Reset();

	LOGMESSAGE(L"\tSet the new buffers' size\n");
	ThrowIfFailed(g_SwapChain->ResizeBuffers(g_SwapChainBuffersCount,
		g_ClientWidth,
		g_ClientHeight,
		g_BackbufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	g_CurrentBackbuffer = 0;

	LOGMESSAGE(L"\tCreate render target views\n");
	CD3DX12_CPU_DESCRIPTOR_HANDLE tmpHeapHandle(g_RtvDescHeap->GetCPUDescriptorHandleForHeapStart()) ;
	for (int i = 0; i < g_SwapChainBuffersCount; ++i) {
		ThrowIfFailed(g_SwapChain->GetBuffer(i, IID_PPV_ARGS(&g_Backbuffers[i])));
		// set the rtv descriptor in the ith buffer
		g_Device->CreateRenderTargetView(g_Backbuffers[i].Get(), nullptr, tmpHeapHandle);
		// move the heap forward
		tmpHeapHandle.Offset(1, g_RtvDescriptorSize);
	}

	LOGMESSAGE(L"\tReset depth/stencil buffer\n");
	CreateDepthStencilBuffer();


	CloseGlobalCommandList();
	FlushGPUCommandsQueueAll();

	LOGMESSAGE(L"\tSet the Viewport\n");
	// describe the viewport
	g_ViewPort = {};
	g_ViewPort.MaxDepth = 1;
	g_ViewPort.MinDepth = 0;
	g_ViewPort.Height = static_cast<float>(g_ClientHeight);
	g_ViewPort.Width = static_cast<float>(g_ClientWidth);
	g_ViewPort.TopLeftX = g_ViewPort.TopLeftY = 0.0f;

	LOGMESSAGE(L"\tDefine Scissors Rectanlges if needed\n");
	g_ScissorsRectangle = { 0,0,static_cast<long>(g_ClientWidth),static_cast<long>(g_ClientHeight) };
}

#pragma endregion Resize

#pragma region Terminate

void Gfx::Terminate() {
	LOGMESSAGE(L"Finalizing graphics ... \n");
	assert(g_Device);
	FlushGPUCommandsQueueAll();
	g_Device.Reset();
}

#pragma endregion Terminate

#pragma region Pipeline

void Gfx::InitializePipeline() {
	
	LOGMESSAGE(L"Initializing Pipeline\n");
	
	FlushGPUCommandsQueueAll();

	ThrowIfFailed(g_CommandAllocator->Reset());
	ThrowIfFailed(g_CommandList->Reset(g_CommandAllocator.Get(), nullptr));
	
	LOGMESSAGE(L"\tDescribing Vertices\n");
	CreateInputLayout();

	LOGMESSAGE(L"\tCreate main camera buffers\n");
	AddFrameConstantBuffers(g_CbvDescHeap);

	LOGMESSAGE(L"\tCreate the Root Signature\n");
	CreateRootSignature();

	LOGMESSAGE(L"\tSet up the Pipeline state\n");
	CreatePSO(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, g_PipelineStateTriangle);
	CreatePSO(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, g_PipelineStateLine);
		
	// choose one and send it to the GPU's commands queue for now, we will alternate it will drawing based on the mesh
	g_CommandList->SetPipelineState(g_PipelineStateTriangle.Get());

	LOGMESSAGE(L"\tFlush the queue\n");
	CloseGlobalCommandList();
	FlushGPUCommandsQueueAll();
	g_CommandAllocator->Reset();
	g_CommandList->Reset(g_CommandAllocator.Get(), nullptr);
}

#pragma endregion Pipeline