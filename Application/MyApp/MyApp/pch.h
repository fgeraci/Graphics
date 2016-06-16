#pragma once

#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "Common\d3dx12.h"
#include <pix.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <agile.h>
#include <concrt.h>
#include <string>

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif

// Our includes
#include <Windows.h>		// Performance counter
#include "DirectXHelper.h"	// ThrowIfFailed
#include "DirectXColors.h"	// DirectX::Colors
#include "Ticker.h"			// FPS controller

/* Debug macro */
#ifdef _DEBUG
#define LOGMESSAGE( str ) OutputDebugString( str );
#else
#define LOGMESSAGE( str )
#endif