#pragma once
#include "pch.h"

using namespace Application;

#pragma region Cube

Cube::Cube(bool d) : Polygon(d) {
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
	g_Name = L"Cube";
	g_PolygonType = POLYGON_TYPE::CUBE;
	g_Topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Entity::InitializeVerticesLists();
}

#pragma endregion Cube

#pragma region Grid

Grid::Grid() : Polygon() {
	/*
	From the origin, the grid will span half to each direction on the x-z half-plane
	The stepping should be based on the screen width/height ratio.
	*/
	PopulateGrid();
	g_Name = L"Grid";
	g_PolygonType = POLYGON_TYPE::GRID;
	g_Topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	Entity::InitializeVerticesLists();
}

void Grid::PopulateGrid() {
	int size = g_Size;
	int start = size % 2 == 0 ? -1 * (size / 2) : -1 * ((--size) / 2);	// start from the bottom left corner
	for (int z = 0; z <= size; ++z) {
		int indexOffset = static_cast<int>(g_Vertices.size());
		for (int x = 0; x <= size; ++x) {
			// create vertices
			Vertex v = { XMFLOAT3(static_cast<float>(x + start), 0.0f, static_cast<float>(z + start)), g_Color };
			g_Vertices.push_back(v);
			// create indices
			int combined = x + indexOffset;
			int combinedPlus = combined + 1;
			if (x < size) {
				g_Indices.push_back(combined);
				g_Indices.push_back(combinedPlus);					// immediate next
			}
			if (z < size) {
				g_Indices.push_back(combined);
				g_Indices.push_back(combinedPlus + size);			// one right above on z
			}
		}
	}
}

#pragma endregion Grid

#pragma region Cylinder

Cylinder::Cylinder() {
	PopulateCylinder();
	g_Name = L"Cylinder";
	g_PolygonType = POLYGON_TYPE::CYLINDER;
	g_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Polygon::InitializeVerticesLists();
}

void Cylinder::PopulateCylinder() {

	float cylHeight = g_Stacks*g_StackHeight;
	float stackHeightConstant = (float)((-1.0f / 2.0f) * cylHeight);
	float sliceAngle = (360.0f / (float) g_Slices) * (XM_PI / 180.0f);
	float radiusDelta = (g_BottomRadius - g_TopRadius);

	int offset = 0;
	
	for(int stack = 0; stack < g_Stacks; stack++) {

		bool center = (stack == 0 || stack == g_Stacks - 1);

		float stackHeight = stackHeightConstant + stack * g_StackHeight;
		float stackRadius = g_BottomRadius - stack * ((g_BottomRadius - g_TopRadius)/g_Stacks);
		int base = stack * g_Slices + offset;

		int shift = (center ? 1 : 0);
		for (int slice = 0; slice < g_Slices; ++slice) {
			Vertex v;
			v.position = XMFLOAT3(stackRadius*cos(sliceAngle*slice), stackHeight, stackRadius*sin(sliceAngle*slice));
			v.color = center ? g_ColorEnds : g_Color;

			g_Vertices.push_back(v);
		
			int cur		= slice + base, 
				curNext = ((cur + 1 - offset) % g_Slices) + base, 
				curUp	= cur + g_Slices + shift,
				curMid	= base + g_Slices,
				curDiag = curNext + g_Slices + shift;
		
			g_Indices.push_back(cur);
			g_Indices.push_back(curUp);
			g_Indices.push_back(curDiag);
			
			
			g_Indices.push_back(cur);
			g_Indices.push_back(curDiag);
			g_Indices.push_back(curNext);
			

			if (center) {
				if(stack == 0) {
					g_Indices.push_back(curNext);
					g_Indices.push_back(curMid);
					g_Indices.push_back(cur);
				} else {
					g_Indices.push_back(cur);
					g_Indices.push_back(curMid);
					g_Indices.push_back(curNext);
				}
			}
		}

		offset += center ? 1 : 0;
	}

	Vertex v1;
	v1.position = XMFLOAT3(0.0f, stackHeightConstant, 0.0f);
	v1.color = g_Color;

	// add the bottom center
	g_Vertices.insert(g_Vertices.begin() + g_Slices, v1);

	Vertex v2;
	v2.position = XMFLOAT3(0.0f, stackHeightConstant + ((g_Stacks-1)*g_StackHeight), 0.0f);
	v2.color = g_Color;

	if(g_Stacks > 1) {
		g_Vertices.insert(g_Vertices.begin() + (g_Stacks*g_Slices)+1, v2);
	}
}

#pragma endregion Cylinder