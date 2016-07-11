#pragma once
#include "pch.h"

using namespace Application;

#pragma region Cube

Cube::Cube() {
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
	g_PolygonType = POLYGON_TYPE::CUBE;
	g_Topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	g_VerticesNumber = static_cast<UINT>(g_Vertices.size());
	g_IndicesNumber = static_cast<UINT>(g_Indices.size());
	g_VBBytesSize = g_VerticesNumber * Polygon::StrideSize;
	g_IBBytesSize = g_IndicesNumber * sizeof(Polygon::IndexFormat);
	g_Name = L"Cube";
}

void Cube::Rotate()		{}
void Cube::Translate(DIRECTION dir, float speed)	{}
void Cube::Scale()		{}

#pragma endregion Cube

#pragma region Grid

Grid::Grid(int size) {
	/* 
		From the origin, the grid will span half to each direction on the x-z half-plane
		The stepping should be based on the screen width/height ratio.
	*/
	int start = size % 2 == 0 ? -1*(size / 2) : -1 * ((--size) / 2);	// start from the bottom left corner
	for (int z = 0; z <= size; ++z) {
		int indexOffset = static_cast<int>(g_Vertices.size());
		for (int x = 0; x <= size; ++x) {
			// create vertices
			Vertex v = { XMFLOAT3(static_cast<float>(x + start), 0.0f, static_cast<float>(z + start)), g_Color };
			g_Vertices.push_back(v);
			// create indices
			int combined = x + indexOffset;
			int combinedPlus = combined + 1;
			if(x < size) { 
				g_Indices.push_back(combined);
				g_Indices.push_back(combinedPlus);					// immediate next
			}
			if (z < size) {
				g_Indices.push_back(combined);
				g_Indices.push_back(combinedPlus + size);			// one right above on z
			}
		}
	}
	g_PolygonType = POLYGON_TYPE::GRID;
	g_Topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	g_VerticesNumber = static_cast<UINT>(g_Vertices.size());
	g_IndicesNumber = static_cast<UINT>(g_Indices.size());
	g_VBBytesSize = g_VerticesNumber * Polygon::StrideSize;
	g_IBBytesSize = g_IndicesNumber * sizeof(Polygon::IndexFormat);
	g_Name = L"Grid";
}

void Grid::Rotate() {}
void Grid::Translate(DIRECTION dir, float speed) {}
void Grid::Scale() {}

#pragma endregion Grid