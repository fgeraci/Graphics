#include "pch.h"
#include "Gfx.h"

using namespace Application;
using namespace DX;

Gfx::Gfx() {

}

void Gfx::InitializeDebugging() {
#if defined(DEBUG) || defined (_DEBUG)
	DX::ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugController)));
	m_DebugController->EnableDebugLayer();
#endif
}

void Gfx::InitializeDirect3d() {
}