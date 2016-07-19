#pragma once

#include "pch.h"

namespace Application {

	class Entity : public ITransformable {
	
	protected:
	
		XMVECTOR g_Up;
		XMVECTOR g_Forward;
		XMVECTOR g_Right;
		XMVECTOR g_Position;
		XMMATRIX g_WorldMatrix = XMMatrixIdentity();
		bool g_Dirty = false;

		std::vector<Vertex> g_Vertices;
		std::vector<Vertex> g_LocalVertices;
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
		bool g_Enabled = true;
		bool g_Drawable = false;
		D3D12_VERTEX_BUFFER_VIEW g_VertexBufferView;
		D3D12_INDEX_BUFFER_VIEW g_IndexBufferView;
		ComPtr<ID3D12Resource> g_GPUVertexBuffer, g_CPUVertexBuffer;
		ComPtr<ID3D12Resource> g_GPUIndexBuffer, g_CPUIndexBuffer;
		std::unique_ptr<UploadBuffer<Vertex>> g_UploadBuffer = nullptr;

		virtual void UpdateWorldMatrix();
		virtual void Translate(DIRECTION, float);
		virtual void Rotate(DIRECTION,float);
		virtual void Scale() {}

	public:	

		Entity();

		virtual void Transform(TRANSFORMATION_TYPE, DIRECTION d, float speed = 1.0f);

		bool IsDirty() { return g_Dirty; }
		void SetDirty(bool d) { g_Dirty = d; }

		virtual void InitializeUploadBuffer(ComPtr<ID3D12Device>);
		virtual void UpdateVertexBuffer();

		virtual XMVECTOR Position()		{ return g_Position; }
		virtual XMVECTOR Up()			{ return g_Up; }
		virtual XMVECTOR Forward()		{ return g_Forward; }
		virtual XMVECTOR Right()		{ return g_Right; }
		virtual XMMATRIX WorldMatrix()	{ return g_WorldMatrix; }

		std::wstring Name();
		UINT VerticesNumber();
		bool IsEnabled();
		bool IsDrawable();
		UINT IndicesNumber();
		UINT VBBytesSize();
		UINT IBBytesSize();
		UINT StartVertexLocation();
		UINT StartIndexLocation();
		std::vector<Vertex> Vertices();
		std::vector<uint16_t> Indices();
		void SetVBView(D3D12_VERTEX_BUFFER_VIEW p);
		void SetIBView(D3D12_INDEX_BUFFER_VIEW p);
		D3D12_VERTEX_BUFFER_VIEW VBView();
		D3D12_INDEX_BUFFER_VIEW IBView();
		D3D12_PRIMITIVE_TOPOLOGY Topology();

	};
}