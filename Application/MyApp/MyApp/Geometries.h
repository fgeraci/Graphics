#pragma once

#include "pch.h"

using namespace Microsoft::WRL;

namespace Application {
	
	enum POLYGON_TYPE {
		CUBE = 1
	};

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

	class Polygon : public Entity {
	
	public:
		
		static const DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		static const UINT StrideSize = sizeof(Vertex);

	protected:

		std::vector<Vertex> g_Vertices;
		std::vector<uint16_t> g_Indices;
		std::wstring g_Name;
		UINT g_VerticesNumber;
		UINT g_IndicesNumber;
		UINT g_VBBytesSize;
		UINT g_IBBytesSize;
		UINT g_StartVertexLocation = 0;
		UINT g_StartIndexLocation = 0;
		D3D12_VERTEX_BUFFER_VIEW g_VertexBufferView;
		D3D12_INDEX_BUFFER_VIEW g_IndexBufferView;

	public:

		virtual void Translate(DIRECTION, float) abstract;
		virtual void Rotate() abstract;
		virtual void Scale() abstract;
		
		std::wstring Name() {
			return g_Name;
		}

		UINT VerticesNumber() {
			return g_VerticesNumber;
		}

		UINT IndicesNumber() {
			return g_IndicesNumber;
		}

		UINT VBBytesSize() {
			return g_VBBytesSize;
		}

		UINT IBBytesSize() {
			return g_IBBytesSize;
		}

		UINT StartVertexLocation() {
			return g_StartVertexLocation;
		}

		UINT StartIndexLocation() {
			return g_StartIndexLocation;
		}

		std::vector<Vertex> Vertices() {
			return g_Vertices;
		}

		std::vector<uint16_t> Indices() {
			return g_Indices;
		}

		void SetVBView(D3D12_VERTEX_BUFFER_VIEW p) {
			g_VertexBufferView = p;
		}

		void SetIBView(D3D12_INDEX_BUFFER_VIEW p) {
			g_IndexBufferView = p;
		}

		D3D12_VERTEX_BUFFER_VIEW VBView() {
			return g_VertexBufferView;
		}

		D3D12_INDEX_BUFFER_VIEW IBView() {
			return g_IndexBufferView;
		}

	};

	class Cube sealed : public Polygon {
	
	public:

		Cube() {
			g_Vertices = {
				{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) },
				{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Black) },
				{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Red) },
				{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) },
				{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Blue) },
				{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(Colors::Yellow) },
				{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(Colors::Cyan) },
				{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Magenta) }
			};
			g_Indices = 
			{
				// front face
				0, 1, 2,
				0, 2, 3,

				// back face
				4, 6, 5,
				4, 7, 6,

				// left face
				4, 5, 1,
				4, 1, 0,

				// right face
				3, 2, 6,
				3, 6, 7,

				// top face
				1, 5, 6,
				1, 6, 2,

				// bottom face
				4, 0, 3,
				4, 3, 7
			};
			g_VerticesNumber = static_cast<UINT>(g_Vertices.size());
			g_IndicesNumber = static_cast<UINT>(g_Indices.size());
			g_VBBytesSize = g_VerticesNumber * Polygon::StrideSize;
			g_IBBytesSize = g_IndicesNumber * sizeof(Polygon::IndexFormat);
			g_Name = L"Cube";
		}

		void Rotate() override;
		void Translate(DIRECTION, float) override;
		void Scale() override;
	};
}
