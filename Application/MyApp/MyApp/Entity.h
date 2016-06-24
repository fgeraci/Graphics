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

		virtual void UpdateWorldMatrix();
		virtual void Translate(DIRECTION, float);
		virtual void Rotate(DIRECTION,float);
		virtual void Scale() {}

	public:	

		Entity();

		virtual void Transform(TRANSFORMATION_TYPE, DIRECTION d, float speed = 1.0f);

		bool IsDirty() { return g_Dirty; }
		void SetDirty(bool d) { g_Dirty = d; }

		virtual XMVECTOR Position()		{ return g_Position; }
		virtual XMVECTOR Up()			{ return g_Up; }
		virtual XMVECTOR Forward()		{ return g_Forward; }
		virtual XMVECTOR Right()		{ return g_Right; }
		virtual XMMATRIX WorldMatrix()	{ return g_WorldMatrix; }
	};
}