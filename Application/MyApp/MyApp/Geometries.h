#pragma once

#include "pch.h"

using namespace Microsoft::WRL;

namespace Application {
	
	enum POLYGON_TYPE {
		GRID = 0,
		CUBE
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

	///
	/// Polygon is the master class of every primitive geometry for basic examples.
	/// Each geometry derives from polygon and initialize its vertices and indices in
	/// different ways.
	///

	class Polygon : public Entity {
	
	public:
		
		virtual ~Polygon() {
			// release all COM references
			g_CPUVertexBuffer = g_GPUVertexBuffer = g_CPUIndexBuffer = g_GPUIndexBuffer = nullptr;
		}

		static const DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		static const UINT StrideSize = sizeof(Vertex);

	protected:

		std::vector<Vertex> g_Vertices;
		std::vector<uint16_t> g_Indices;
		std::wstring g_Name;
		D3D12_PRIMITIVE_TOPOLOGY g_Topology;
		bool g_Dynamic = false;							// determine buffer allocation on build
		UINT g_VerticesNumber;
		UINT g_IndicesNumber;
		UINT g_VBBytesSize;
		UINT g_IBBytesSize;
		UINT g_StartVertexLocation = 0;
		UINT g_StartIndexLocation = 0;
		POLYGON_TYPE g_PolygonType;
		bool g_Visible = true;
		D3D12_VERTEX_BUFFER_VIEW g_VertexBufferView;
		D3D12_INDEX_BUFFER_VIEW g_IndexBufferView;
		ComPtr<ID3D12Resource> g_GPUVertexBuffer, g_CPUVertexBuffer;
		ComPtr<ID3D12Resource> g_GPUIndexBuffer, g_CPUIndexBuffer;

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

		D3D12_PRIMITIVE_TOPOLOGY Topology() {
			return g_Topology;
		}

		POLYGON_TYPE PolygonType() {
			return g_PolygonType;
		}

		bool IsVisible() {
			return g_Visible;
		}

		void SetVisible(bool vis) {
			g_Visible = vis;
		}


		bool IsDynamic() { 
			return g_Dynamic; 
		}
		
		void SetDynamic(bool dyn) { 
			g_Dynamic = dyn; 
		}

		void SetGPUVertexBuffer(ComPtr<ID3D12Resource> buf) {
			g_GPUVertexBuffer = buf;
		}

		void SetCPUVertexBuffer(ComPtr<ID3D12Resource> buf) {
			g_CPUVertexBuffer = buf;
		}

		void SetGPUIndexBuffer(ComPtr<ID3D12Resource> buf) {
			g_GPUIndexBuffer = buf;
		}

		void SetCPUIndexBuffer(ComPtr<ID3D12Resource> buf) {
			g_CPUIndexBuffer = buf;
		}

		ComPtr<ID3D12Resource> CPUVertexBuffer() { return g_CPUVertexBuffer; }
		ComPtr<ID3D12Resource> GPUVertexBuffer() { return g_GPUVertexBuffer; }
		ComPtr<ID3D12Resource> CPUIndexBuffer() { return g_CPUIndexBuffer; }
		ComPtr<ID3D12Resource> GPUIndexBuffer() { return g_GPUIndexBuffer; }

	};

	class Cube sealed : public Polygon {
	
	public:

		Cube();
		Cube(bool d) : Cube() { g_Dynamic = d; }

		void Rotate() override;
		void Translate(DIRECTION, float) override;
		void Scale() override;
	};


	/// A grid will be the reference 'floor' of our 3D world at the y=0 height expanded on the x,z half-plane.
	/// A dynamic grid will always count with 'size' divisions through out any position of the camera, while
	/// a static one, will only have 'size' blocks which will not adapt to the projection screen.

	class Grid sealed : public Polygon {

	private:
		
		
		XMFLOAT4 g_Color = XMFLOAT4(Colors::White);

	public:

		Grid(int);
		Grid() : Grid(25) { }
		Grid(int s, bool d) : Grid(s) { g_Dynamic = d; }
		Grid(int s, DirectX::XMFLOAT4 c) : Grid(s) { g_Color = c; }
		Grid(int s, bool d, DirectX::XMFLOAT4 c) : Grid(s, d) { g_Color = c; }

		void Rotate() override;
		void Translate(DIRECTION, float) override;
		void Scale() override;

	};
}