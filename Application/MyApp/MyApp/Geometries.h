#pragma once

#include "pch.h"

using namespace Microsoft::WRL;

namespace Application {
	
	enum POLYGON_TYPE {
		GRID = 0,
		CUBE,
		CYLINDER
	};

	class Polygon : public Entity {
	
	public:
		
		Polygon() { g_Drawable = true; }
		Polygon(bool d) : Polygon() { g_Dynamic = d; }

		virtual ~Polygon() {
			// release all COM references
			g_CPUVertexBuffer = g_GPUVertexBuffer = g_CPUIndexBuffer = g_GPUIndexBuffer = nullptr;
		}

		static const DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		static const UINT IndexSize = sizeof(uint16_t);
		static const UINT StrideSize = sizeof(Vertex);
		static const UINT DefaultGridSize = 70;

	protected:

		POLYGON_TYPE g_PolygonType;
		
	public:

		POLYGON_TYPE PolygonType() {
			return g_PolygonType;
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
		Cube() : Cube(false) { Polygon(); }
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

	class Cylinder sealed : public Polygon {
	
	private:

		XMFLOAT4 g_Color = XMFLOAT4(Colors::Green);
		XMFLOAT4 g_ColorEnds = XMFLOAT4(Colors::DarkGreen);
		int	  g_Stacks = 8;
		float g_StackHeight = 0.2f;
		float g_BottomRadius = 1.0f;
		float g_TopRadius = 0.5f;
		int   g_Slices = 16;
	
		void PopulateCylinder();

	public:

		Cylinder();
		Cylinder(bool d) : Cylinder() { g_Dynamic = d; }
		Cylinder(int,float,float,int) { };

	};
}