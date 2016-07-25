#pragma once

#include "pch.h"

using namespace Application;

Entity::Entity() {
	g_Position	= XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	g_Up		= m_WorldUpVector;
	g_Right		= m_WorldRightVector;
	g_Forward	= m_WorldForwardVector;
}

#pragma region Transformations

void Entity::UpdateWorldMatrix() {
	g_Right = XMVector3Normalize(g_Right);
	g_Forward = XMVector3Normalize(g_Forward);
	g_Up = XMVector3Normalize(XMVector3Cross(g_Forward, g_Right));
	g_Right = XMVector3Cross(g_Up, g_Forward);
	g_WorldMatrix = XMMATRIX(g_Right, g_Up, g_Forward, g_Position);
}

void Entity::InitializeVerticesLists() {
	g_VerticesNumber = static_cast<UINT>(g_Vertices.size());
	g_IndicesNumber = static_cast<UINT>(g_Indices.size());
	g_VBBytesSize = g_VerticesNumber * Polygon::StrideSize;
	g_IBBytesSize = g_IndicesNumber * Polygon::IndexSize;
}

void Entity::Transform(TRANSFORMATION_TYPE t, TRANSFORM_DIRECTION d, TRANSFORM_HIERARCHY h, float mod) {
	
	switch(t) {
	case TRANSFORMATION_TYPE::TRANSLATE:
		Translate(d,h, mod);
		break;
	case TRANSFORMATION_TYPE::ROTATE:
		Rotate(d, h, mod);
		break;
	}
	
	// Update world matrix
	UpdateWorldMatrix();

	// transform children one by one
	// Translation is trivial
	// rotation needs to determine: parent axis of rotation and position relative to parent
	// TODO - resolve parent's coordinates and axes
	for (int i = 0; i < g_Children.size(); ++i) {
		g_Children[i]->Transform(t, d, TRANSFORM_HIERARCHY::PARENT, mod);
	}
}

void Entity::Translate(TRANSFORM_DIRECTION d, TRANSFORM_HIERARCHY h, float speed) {
	// Concept: g_position += speed * forward
	XMVECTOR delta = XMVectorReplicate(speed);
	float dir = -1;
	switch(d) {
	case TRANSFORM_DIRECTION::FORWARD:
	case TRANSFORM_DIRECTION::RIGHT:
	case TRANSFORM_DIRECTION::UP: 
	case TRANSFORM_DIRECTION::STRAFE_RIGHT:
		dir = 1.0f;
	}
	XMVECTOR mainAxis;
	if (d == TRANSFORM_DIRECTION::FORWARD || d == TRANSFORM_DIRECTION::BACKWARDS) {
		mainAxis = g_Forward;
	} else if (d == TRANSFORM_DIRECTION::UP || d == TRANSFORM_DIRECTION::DOWN) {
		mainAxis = g_Up;
	} else mainAxis = g_Right;
	g_Position = XMVectorMultiplyAdd(delta, dir * mainAxis, g_Position);
	/*for(UINT i = 0; i < g_VerticesNumber; ++i) {
		Math::MultiplyAdd(speed, dir * mainAxis, (g_Vertices[i].position));
	}*/
}

void Entity::Rotate(TRANSFORM_DIRECTION d, TRANSFORM_HIERARCHY h, float angle) {
	
	int dir = m_LeftScreenSide;
	bool local, parent, world;;
	local = parent = world = false;
	
	if(h == TRANSFORM_HIERARCHY::LOCAL) {
		local = true;
	} else if(h == TRANSFORM_HIERARCHY::PARENT) {
		parent = true;
	}
	
	switch (d) {
	case TRANSFORM_DIRECTION::UP: 
	case TRANSFORM_DIRECTION::LEFT:
	case TRANSFORM_DIRECTION::STRAFE_LEFT:
		dir = m_RightScreenSide;
	}

	XMVECTOR axis;
	
	// 1. Select the proper rotation vector, presedence is local, parent and world.
	switch(d) {
	case TRANSFORM_DIRECTION::RIGHT:			// yaw 
	case TRANSFORM_DIRECTION::LEFT:
		axis	= local	 ? g_Up : (parent ? g_Parent->Up() : m_WorldUpVector);
		break;
	case TRANSFORM_DIRECTION::STRAFE_RIGHT:		// roll
	case TRANSFORM_DIRECTION::STRAFE_LEFT:
		axis	= local ? g_Forward : (parent ? g_Parent->Forward() : m_WorldForwardVector);
		break;
	case TRANSFORM_DIRECTION::UP:				// pitch
	case TRANSFORM_DIRECTION::DOWN:
		axis	= local ? g_Right : (parent ? g_Parent->Right() : m_WorldRightVector);
	}
	
	XMMATRIX r, t;
	XMFLOAT4X4 t4, r4;
	XMFLOAT3 parentOffset;

	if(angle > 1.0f) {
		r = XMMatrixRotationAxis(axis, (angle * XM_PI) / 180.00f);
	} else 
		r = (dir == 1) ? Math::GetRotationMatrixForAxis(axis) : XMMatrixTranspose(Math::GetRotationMatrixForAxis(axis));
	
	g_Up = XMVector3Normalize(XMVector4Transform(g_Up, r));
	g_Forward = XMVector3Normalize(XMVector4Transform(g_Forward, r));
	g_Right = XMVector3Normalize(XMVector4Transform(g_Right, r));

	if(local) {
		t = Math::GetTranslationMatrix(g_Position);
		XMStoreFloat4x4(&t4, t);	
	} else {
		parentOffset = XMFLOAT3(0, 0, 0);
		//XMStoreFloat3(&parentOffset, g_Parent->Position());
		g_Position = XMVector4Transform(g_Position, r);
	}
	XMStoreFloat4x4(&r4, r);

	for (UINT i = 0; i < g_VerticesNumber; ++i) {
		if(local) {
			// rotate the local vertices
			g_Vertices[i].position = Math::Transform3(g_Vertices[i].position, r4);
		} else {
			// rotate around a world/parent axis
			Math::RotateOnAxis(g_Vertices[i].position, r4, dir);
		}
	}
}

#pragma endregion Transformations


void Entity::InitializeUploadBuffer(ComPtr<ID3D12Device> dev) {
	g_UploadBuffer = std::make_unique<UploadBuffer<Vertex>>(dev, g_VerticesNumber, false);
	g_CPUVertexBuffer = g_UploadBuffer->GetResource();
}

void Entity::UpdateBuffers() {
	if(g_Dynamic) {
		g_UploadBuffer->WriteToBuffer(g_Vertices[0], g_StartVertexLocation);
		g_GPUVertexBuffer.Reset();
		g_GPUVertexBuffer = g_UploadBuffer->GetResource();
		g_VertexBufferView.BufferLocation = g_GPUVertexBuffer->GetGPUVirtualAddress();
		g_Dirty = false;
		ObjectConstatntData data;
		XMStoreFloat4x4(&data.gWorld, XMMatrixTranspose(g_WorldMatrix));
		g_ConstantBuffer->WriteToBuffer(data, 0);
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

void Entity::InitConstantBuffer(ComPtr<ID3D12Device> dev) {
	
	// store the pointer
	g_ConstantBuffer = std::make_unique<UploadBuffer<ObjectConstatntData>>(dev, 1, true);

	// Initialize the world matrix here in case this is 
	// a non-dynamic per frame object(such as the grid)
	UpdateWorldMatrix();
	ObjectConstatntData data;
	XMStoreFloat4x4(&data.gWorld, XMMatrixTranspose(g_WorldMatrix));
	g_ConstantBuffer->WriteToBuffer(data, 0);
}

D3D12_VERTEX_BUFFER_VIEW Entity::VBView() {
	return g_VertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW Entity::IBView() {
	return g_IndexBufferView;
}

UploadBuffer<ObjectConstatntData>* Entity::ConstantBuffer() {
	return g_ConstantBuffer.get();
}

D3D12_PRIMITIVE_TOPOLOGY Entity::Topology() {
	return g_Topology;
}

#pragma endregion Accessors/Mutators