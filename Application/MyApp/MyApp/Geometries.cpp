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

#pragma region Icosahedron

Icosahedron::Icosahedron() {
	
	g_TriangleHeight = sqrt(pow(g_SideSize,2.0f) - pow(((1 / 2.0f)*g_SideSize),2.0f));
	float totalDistance = (g_TriangleHeight * 1.5f);

	XMMATRIX r30x = XMMatrixRotationAxis(m_WorldRightVector, 30 * (XM_PI / 180.0f));
	XMMATRIX t = XMMatrixTranslation(0.0f, 0.75f * totalDistance, 0.0f);

	for(int i = 0; i < 5; i++) {

		// look forward on z
		XMFLOAT4 curVertex = XMFLOAT4(0.0f, 0.0f, g_SideSize, 1.0f);
		XMVECTOR v = XMVector3Transform(XMLoadFloat4(&curVertex), r30x);
		
		if(i > 0) {
			XMMATRIX onY = XMMatrixRotationAxis(m_WorldUpVector,(72*i) * (XM_PI / 180.0f));
			v = XMVector3Transform(v, onY);
		}

		v = XMVector3Transform(v, t);

		Vertex vertex;
		vertex.color = g_Colors[i % 4];
		XMFLOAT4 pos;
		XMStoreFloat4(&pos, v);
		vertex.position = XMFLOAT3(pos.x, pos.y, pos.z);
		g_Vertices.push_back(vertex);

		g_Indices.push_back(i);
		g_Indices.push_back((i + 1) % 5);
		g_Indices.push_back(5);

	}

	// top vertex
	XMFLOAT3 C = XMFLOAT3(0.0f, 0.75f * totalDistance, 0.0f);
	Vertex vertexC;
	vertexC.position = C;
	vertexC.color = g_Colors[3];
	g_Vertices.push_back(vertexC);

	int length = static_cast<int>(g_Vertices.size());
	int offset = 6; // 5 previous vertices plus C

	for(int v = offset; v < length+offset-1; ++v) {
		Vertex vBottom;
		Vertex vTop = g_Vertices[v-offset];
		vBottom.position = XMFLOAT3(vTop.position.x, -1.0f*(vTop.position.y), -1.0f*vTop.position.z);
		vBottom.color = g_Colors[v % 4];
		g_Vertices.push_back(vBottom);

		g_Indices.push_back(v);
		g_Indices.push_back((v % 5) + offset);
		g_Indices.push_back(11);
	}

	// bottom vertex
	XMFLOAT3 B = XMFLOAT3(0.0f, -1.0f*(0.75f * totalDistance), 0.0f);
	Vertex vertexB;
	vertexB.position = B;
	vertexB.color = g_Colors[0];
	g_Vertices.push_back(vertexB);

	for(int i = 0; i < 5; ++i) {

		int next = (i + 1) % 5;
		int prevCounterClock = 8 - i > 5 ? 8 - i : 8 - i + 5;
		int nextCounterClock = 8 - i - 1 > 5 ? 8 - i - 1 : 8 - i - 1 + 5;
		
		g_Indices.push_back(i);
		g_Indices.push_back(prevCounterClock);
		g_Indices.push_back(next);

		g_Indices.push_back(prevCounterClock);
		g_Indices.push_back(nextCounterClock);
		g_Indices.push_back(next);

	}

	Triangulate();
	g_Name = L"Icosahedron";
	g_PolygonType = POLYGON_TYPE::ICOSAHEDRON;
	g_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Polygon::InitializeVerticesLists();
}

void Icosahedron::Triangulate() {

	// Pentagons - from a single pentagon, we can derive 
	// all the vertices
	int currentCenter		= g_Indices[2];									// always at the top of the main inner pentagon
	int topPentagonV		= static_cast<int>(g_Vertices.size()*0.5f);
	int topPentagonIn		= static_cast<int>(g_Indices.size()*0.25f);
	long totalIndices		= (topPentagonIn) * 4;
	int totalTriangles		= (totalIndices / 3);
	int currentIteration	= (static_cast<int>(floor(log(topPentagonIn) / log(5))));
	int totalVertices		= (totalTriangles / 2) + 5 * currentIteration;
	float initAngle			= (72.0f / static_cast<float>(currentIteration));
	int layerVertices		= static_cast<int>(360 / (initAngle / 2));
	
	// recreate vertices and swap pointers
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	int sum = 0;
	int verticesToAdd = totalVertices - topPentagonV + 1; // +1 current  center
	int totalRounds = 0;
	while(sum < verticesToAdd) {
		sum += 360 / (72 / (totalRounds + 2)); // each round we divide the angle
		totalRounds++;
	}

	// 1. Add the main pentagons indices
	for (int i = 0; i < topPentagonIn; i++) 
		indices.push_back(g_Indices[i]);

	// 2. adjust exiting top vertices
	Vertex va;
	float factor = 2.0f * static_cast<float>(totalRounds);
	float yMod = 1.0f + (1.0f / factor);
	for(int top = 0; top < topPentagonV; ++top) {
		if(top != currentCenter) {
			// top
			XMFLOAT3 origTop = g_Vertices[top].position;
			// shrink the current vertices by the proper factor
			va.position = XMFLOAT3(origTop.x / factor, origTop.y * yMod, origTop.z / factor);
			va.color = g_Vertices[top].color;
			vertices.push_back(va);
			
		} else vertices.push_back(g_Vertices[top]); // do not change the center, just add it
	}

	// 3. add new vertices
	float angleSum = 0.0f;
	float currRound = 1;
	int layerOffset = 5 * currentIteration + 1;	// we add the center of the pentagon as +1
	float totalDistance = (g_TriangleHeight * 1.5f);
	XMMATRIX r30x = XMMatrixRotationX(Math::ToRadians(30.0f));
	XMMATRIX t = XMMatrixTranslation(0.0f, 0.75f * totalDistance, 0.0f);

	for(int i = 0; i < verticesToAdd; i++) {
		
		Vertex vP1, vM, vP2;
		
		// a. Find extreme pentagon points
		float progress	= angleSum / 72.0f;		// current progress around the pentagon's angles
		int totalVerticesPlusLayer = layerVertices + layerOffset;
		int lastLayerVertices = layerVertices - 5;
		int lowerPoint	= static_cast<int>(floor(progress));	// position of the last full pentagon point
		float p1Angle	= initAngle * lowerPoint;	// Lower one of the 5 main vertices of the pentagon, not in between
		float p2Angle	= static_cast<float>(initAngle * (lowerPoint + 1));		// Top one - round up after 360 
		XMMATRIX xRotP1 = XMMatrixRotationY(Math::ToRadians(p1Angle));
		XMMATRIX xRotP2 = XMMatrixRotationY(Math::ToRadians(p2Angle));
		bool corner		= (i == 0) || (i % (currentIteration + 1)) == 0;
			
			// point 1
		XMVECTOR p1;
		p1 = XMVectorSet(0.0f, 0.0f, static_cast<float>(g_SideSize / totalRounds), 1.0f);
		p1 = XMVector3Transform(p1, r30x);
		p1 = XMVector3Transform(p1, xRotP1);
		p1 = XMVector3Transform(p1, t);
			// point 2 -> reuse point 1 and rotate
		XMVECTOR p2 = XMVectorSet(0.0f, 0.0f, static_cast<float>(g_SideSize / totalRounds), 1.0f);
		p2 = XMVector3Transform(p2, r30x);
		p2 = XMVector3Transform(p2, xRotP2);
		p2 = XMVector3Transform(p2, t);
		
		// b. Find actual vertex
		XMVECTOR vMiddle = XMVectorDivide(XMVectorAdd(p2, p1), XMVectorReplicate(2.0f));
		
		XMFLOAT4 p14, pM4, p24;
		XMStoreFloat4(&p14, p1);
		XMStoreFloat4(&pM4, vMiddle);
		XMStoreFloat4(&p24, p2);

		vP1.position = XMFLOAT3(p14.x, p14.y, p14.z);
		vM.position	 = XMFLOAT3(pM4.x, pM4.y, pM4.z);
		vP2.position = XMFLOAT3(p24.x, p24.y, p24.z);
		
		vP1.color = g_Colors[1];
		vM.color = g_Colors[2];
		vP2.color = g_Colors[3];

		// Add the new vertices
		if(corner) {
			vertices.push_back(vP1);
			i++;					// we inserted two at this point
		}
		vertices.push_back(vM);
		

		// add indices in pairs
			// p1 indices
		int pt1 = (i + layerOffset - (corner ? 1 : 0)) % totalVerticesPlusLayer;
		int med = pt1 + 1;
		int pt2 = ((i + 1) % layerVertices) + layerOffset;
		int below = (corner ? pt1 - static_cast<int>((layerOffset + (angleSum / initAngle)))
			:  pt1 - layerOffset)		// if it is corner, below is one back
			% lastLayerVertices ;		// the layer from below will be from indices 0 < current < lastLayerVertices
		int belowPrev = (below - 1);
		int belowNext = (below + 1) % lastLayerVertices;
		
		// adjust parameters
		angleSum += initAngle;
		if(angleSum >= 360.0f) {	
			currentIteration++;			// layer index on the pentagon
			totalRounds--;				// decrement the number of rounds left
			angleSum = 0;				// reset the angleSum
			initAngle /= 2;				// divide the angle interval for the next round
			layerOffset *= 2;			// the distance in vertices between the current first 
										// and the first below is (5*i + 1) counting center
		}

		/* 
			First we need to check whether we are in a coner vertex
			If so, we triangulate first facing down, then up
			otherwise, we triangulate up then down for each pair
			of vertices.
		*/

		if(corner) {
			// top and bottom
			// corner forwar triangle
			indices.push_back(pt1);
			indices.push_back(med);
			indices.push_back(below);
			
			//first after corner odd
			indices.push_back(med);
			indices.push_back(belowNext);
			indices.push_back(below);

			// last upward
			indices.push_back(med);
			indices.push_back(pt2);
			indices.push_back(belowNext);
		} else {
			// upside down
			indices.push_back(belowPrev);
			indices.push_back(below);
			indices.push_back(pt1);

			// next regular
			indices.push_back(below);
			indices.push_back(med);
			indices.push_back(pt1);
		}

	}

	// 4. bottom - just flip the vertices
	totalVertices += 1; // add the center of the top pentagon back
	for (int i = 0; i < totalVertices; ++i) {
		// bottom
		Vertex vb;
		XMFLOAT3 top = vertices[i].position;
		vb.position = XMFLOAT3(top.x, -1 * top.y, -1 * top.z);
		vb.color = g_Vertices[i % topPentagonV].color;
		vertices.push_back(vb);
	}
	
	// 4. vertices
	int topIndices = indices.size();
	for (int i = 0; i < topIndices; i++)
		indices.push_back(indices[i] + totalVertices);

	// 5. Triangulate sides
	//topPentagonV -= 1; // subtract the center - just keep the side vertices
	//for (int i = 0; i < topPentagonV; ++i) {

	//	int next = (i + 1) % topPentagonV;
	//	int prevCounterClock = totalVertices - i > topPentagonV ? totalVertices - i : totalVertices - i + topPentagonV;
	//	int nextCounterClock = totalVertices - i - 1 > topPentagonV ? totalVertices - i - 1 : totalVertices - i - 1 + topPentagonV;

	//	g_Indices.push_back(i);
	//	g_Indices.push_back(prevCounterClock);
	//	g_Indices.push_back(next);

	//	g_Indices.push_back(prevCounterClock);
	//	g_Indices.push_back(nextCounterClock);
	//	g_Indices.push_back(next);

	//}

	g_Vertices = vertices;
	g_Indices = indices;
}

#pragma endregion Icosahedron