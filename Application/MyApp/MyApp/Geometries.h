#pragma once

#include "pch.h"

using namespace Microsoft::WRL;

namespace Application {
	
	enum POLYGON_TYPE {
		GRID = 0,
		CUBE
	};

	class Polygon : public Entity {
	
	public:
		
		Polygon() { }
		Polygon(bool d) { g_Dynamic = d; }

		virtual ~Polygon() {
			// release all COM references
			g_CPUVertexBuffer = g_GPUVertexBuffer = g_CPUIndexBuffer = g_GPUIndexBuffer = nullptr;
		}

		static const DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		static const UINT IndexSize = sizeof(uint16_t);
		static const UINT StrideSize = sizeof(Vertex);
		static const UINT DefaultGridSize = 50;

	protected:

		POLYGON_TYPE g_PolygonType;
		
	public:

		POLYGON_TYPE PolygonType() {
			return g_PolygonType;
		}

		bool IsEnabled() {
			return g_Enabled;
		}

		void SetVisible(bool vis) {
			g_Enabled = vis;
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
		Cube() : Cube(false) { }
		Cube(bool d);
	};


	/// A grid will be the reference 'floor' of our 3D world at the y=0 height expanded on the x,z half-plane.
	/// A dynamic grid will always count with 'size' divisions through out any position of the camera, while
	/// a static one, will only have 'size' blocks which will not adapt to the projection screen.

	class Grid sealed : public Polygon {

	private:
		
		XMFLOAT4 g_Color = XMFLOAT4(Colors::LightGreen);
		void PopulateGrid();
		int g_Size = DefaultGridSize;

	public:

		Grid();
		Grid(int size) : g_Size(size) { Grid(); }
		Grid(bool d) : Grid() { g_Dynamic = d; }
		Grid(int s, bool d) : Grid(s) { g_Dynamic = d; }
		Grid(int s, DirectX::XMFLOAT4 c) : Grid(s) { g_Color = c; }
		Grid(int s, bool d, DirectX::XMFLOAT4 c) : Grid(s, d) { g_Color = c; }

	};
}