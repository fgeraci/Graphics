#pragma once

#include "pch.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Application::Enums::Transformation;

namespace Application {
	
	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	struct VertexTex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 text1;
		DirectX::XMFLOAT2 text2;
	};

	struct ObjectConstantData {
		XMFLOAT4X4 WorldViewProj = Math::MatrixIdentity();	// from MathUtils.h - a small helper we added
	};

	struct Transform {
		Transform(float f, TRANSFORM_HIERARCHY h, TRANSFORMATION_TYPE t, TRANSFORM_DIRECTION d) 
			: modifier(f), hierarchy(h), type(t), direction(d) {}
		float				modifier;
		TRANSFORM_HIERARCHY hierarchy;
		TRANSFORMATION_TYPE type;
		TRANSFORM_DIRECTION direction;
	};

	struct CommandListHelper {
		ComPtr<ID3D12GraphicsCommandList>	commandList;
		ComPtr<ID3D12CommandAllocator>		commandAllocator;
		UINT64								fence;
	};

}
