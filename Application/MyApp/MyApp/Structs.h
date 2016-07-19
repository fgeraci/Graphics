#pragma once

#include "pch.h"

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

}
