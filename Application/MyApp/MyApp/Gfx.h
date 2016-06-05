#pragma once
#include "pch.h"

using namespace Microsoft::WRL;

namespace Application {
	
	ref class Gfx sealed {
	
	public:
		Gfx();
	private:

		ComPtr<ID3D12Debug> m_DebugController;

		void InitializeDebugging();
		void InitializeDirect3d();
	};

}
