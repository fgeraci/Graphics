#pragma once

#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "Common\d3dx12.h"
#include <pix.h>
#include <memory>
#include <vector>
#include <agile.h>
#include <concrt.h>
#include <string>

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif

// DirectX
#include <DirectXMath.h>
#include <DirectXColors.h>			// Colors
#include <DirectXPackedVector.h>	// XMCOLOR
#include <d3d12.h>					// Input Vertex Classification

// Our includes
#include <Windows.h>				// Performance counter
#include <fstream>					// Read system binary files
#include "D3Dcompiler.h"			// File to blobs - bytestreams
#include <array>					// Generic more convenient arrays
#include <math.h>					// good old math
#include "DirectXHelper.h"			// ThrowIfFailed
#include "DirectXColors.h"			// DirectX::Colors
	// order matters, MyApp uses Gfx
#include "Constants.h"				// Application-wide constants
#include "MathUtils.h"				// Matrix / Vectors math
#include "UploadBuffer.h"			// Resources wrapper class
#include "ITransformable.h"			// All transformable objects
#include "Entity.h"					// All objects wrapper
#include "Geometries.h"				// Geometry Helper - Vertices and Polygons
#include "Camera.h"					// Camera entity
#include "Ticker.h"					// FPS controller
#include "Gfx.h"					// Graphics driver - API manager
#include "MyApp.h"					// Our Main Application


/* Debug macro */
#ifdef _DEBUG
#define LOGMESSAGE( str ) OutputDebugString( str );
#else
#define LOGMESSAGE( str )			// do nothing
#endif