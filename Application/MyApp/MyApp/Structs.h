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

	struct FrameConstantData {
		XMFLOAT4X4	View;
		XMFLOAT4X4	InvView;
		XMFLOAT4X4	Proj;
		XMFLOAT4X4	InvProj;
		XMFLOAT4X4	ViewProj = Math::MatrixIdentity();
		XMFLOAT4X4	InvViewProj;
		XMFLOAT3	EyePosW;
		float		cbPerObjectPad1;
		XMFLOAT2	RTSize;
		XMFLOAT2	InvRTSize;
		float		NearZ;
		float		FarZ;
		float		TotalTime;
		float		DeltaTime;
	};

	struct ObjectConstatntData {
		XMFLOAT4X4 gWorld;
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
