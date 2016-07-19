#pragma once

#include "pch.h"

using namespace Application;

Entity::Entity() {
	g_Position	= XMVectorZero();
	g_Up		= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_Right		= XMVectorSet(1.0, 0.0f, 0.0f, 0.0f);
	g_Forward	= XMVectorSet(0, 0, 1.0f, 0);
}

#pragma region Transformations

void Entity::UpdateWorldMatrix() {
	g_Right = XMVector3Normalize(g_Right);
	g_Forward = XMVector3Normalize(g_Forward);
	g_Up = XMVector3Normalize(XMVector3Cross(g_Forward, g_Right));
	g_Right = XMVector3Cross(g_Up, g_Forward);
}

void Entity::Transform(TRANSFORMATION_TYPE t, DIRECTION d, float speed) {
	switch(t) {
	case TRANSLATE:
		Translate(d, speed);
		break;
	case ROTATE:
		Rotate(d,speed);
		break;
	}
}

void Entity::Translate(DIRECTION d, float speed) {
	// Concept: g_position += speed * forward
	XMVECTOR delta = XMVectorReplicate(speed);
	float dir = -1;
	switch(d) {
	case WORLD_FORWARD:case WORLD_UP:case WORLD_STRAFE_RIGHT:
	case LOCAL_FORWARD:case LOCAL_UP:case LOCAL_STRAFE_RIGHT:
	case WORLD_RIGHT:case LOCAL_RIGHT:
		dir = 1.0f;
	}
	XMVECTOR mainAxis;
	if (d == WORLD_FORWARD || d == WORLD_BACKWARDS) {
		mainAxis = g_Forward;
	} else if (d == WORLD_UP || d == WORLD_DOWN) {
		mainAxis = g_Up;
	} else mainAxis = g_Right;
	g_Position = XMVectorMultiplyAdd(delta, dir * mainAxis, g_Position);
	for(UINT i = 0; i < g_VerticesNumber; ++i) {
		Math::MultiplyAdd(speed, dir * mainAxis, (g_Vertices[i].position));
	}
}

void Entity::Rotate(DIRECTION d, float speed = 1.0f) {
	// this function pointer will get the proper call
	int dir = m_LeftScreenSide;
	bool local = true;
	switch (d) {
	case WORLD_LEFT:case LOCAL_LEFT:
	case WORLD_UP: case LOCAL_UP:
		dir = m_RightScreenSide;
	}
	XMVECTOR axis;
	switch(d) {
	case WORLD_RIGHT: case WORLD_LEFT: 
		local = false;
	case LOCAL_RIGHT: case LOCAL_LEFT:
		axis	= (d == WORLD_RIGHT || d == WORLD_LEFT)	 ? m_WorldUpVector : g_Up;
		break;
	case WORLD_UP: case WORLD_DOWN: 
		local = false;
	case LOCAL_UP: case LOCAL_DOWN:
		axis	= g_Right;
	}
	
	XMMATRIX r, t;
	XMFLOAT4X4 t4, r4;

	if(speed > 1.0f) {
		r = XMMatrixRotationAxis(axis, (speed * XM_PI) / 180.00f);
	} else 
		r = (dir == 1) ? Math::GetRotationMatrixForAxis(axis) : XMMatrixTranspose(Math::GetRotationMatrixForAxis(axis));
	
	g_Up = XMVector3Normalize(XMVector4Transform(g_Up, r));
	g_Forward = XMVector3Normalize(XMVector4Transform(g_Forward, r));
	g_Right = XMVector3Normalize(XMVector4Transform(g_Right, r));

	if(local) {
		t = Math::GetTranslationMatrix(g_Position);
		XMStoreFloat4x4(&t4, t);	
	} else {
		g_Position = XMVector4Transform(g_Position, r);
	}
	XMStoreFloat4x4(&r4, r);

	for (UINT i = 0; i < g_VerticesNumber; ++i) {
		if(local) {
			// update the local vertices
			g_LocalVertices[i].position = Math::Transform3(g_LocalVertices[i].position, r4);
			// put them in the right spot
			g_Vertices[i].position = Math::Transform3(g_LocalVertices[i].position, t4);
		} else {
			Math::RotateOnAxis(g_Vertices[i].position, r4, dir);
		}
	}
}

#pragma endregion Transformations


void Entity::InitializeUploadBuffer(ComPtr<ID3D12Device> dev) {
	g_UploadBuffer = std::make_unique<UploadBuffer<Vertex>>(dev, g_VerticesNumber, false);
	g_CPUVertexBuffer = g_UploadBuffer->GetResource();
}

void Entity::UpdateVertexBuffer() {
	if(g_Dynamic) {
		g_UploadBuffer->WriteToBuffer(g_Vertices[0], g_StartVertexLocation);
		g_GPUVertexBuffer.Reset();
		g_GPUVertexBuffer = g_UploadBuffer->GetResource();
		g_VertexBufferView.BufferLocation = g_GPUVertexBuffer->GetGPUVirtualAddress();
		g_Dirty = false;
	}
}

#pragma region Accessors/Mutators

std::wstring Entity::Name() {
	return g_Name;
}

bool Entity::IsEnabled() {
	return g_Enabled;
}

bool Entity::IsDrawable() {
	return g_Drawable;
}

UINT Entity::VerticesNumber() {
	return g_VerticesNumber;
}

UINT Entity::IndicesNumber() {
	return g_IndicesNumber;
}

UINT Entity::VBBytesSize() {
	return g_VBBytesSize;
}

UINT Entity::IBBytesSize() {
	return g_IBBytesSize;
}

UINT Entity::StartVertexLocation() {
	return g_StartVertexLocation;
}

UINT Entity::StartIndexLocation() {
	return g_StartIndexLocation;
}

std::vector<Vertex> Entity::Vertices() {
	return g_Vertices;
}

std::vector<uint16_t> Entity::Indices() {
	return g_Indices;
}

void Entity::SetVBView(D3D12_VERTEX_BUFFER_VIEW p) {
	g_VertexBufferView = p;
}

void Entity::SetIBView(D3D12_INDEX_BUFFER_VIEW p) {
	g_IndexBufferView = p;
}

D3D12_VERTEX_BUFFER_VIEW Entity::VBView() {
	return g_VertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW Entity::IBView() {
	return g_IndexBufferView;
}

D3D12_PRIMITIVE_TOPOLOGY Entity::Topology() {
	return g_Topology;
}

#pragma endregion Accessors/Mutators