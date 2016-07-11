#pragma once

#include "pch.h"

namespace Application {
	
	class Entity : public ITransformable {
	
	protected:
	
		XMVECTOR g_Up;
		XMVECTOR g_Forward;
		XMVECTOR g_Right;
		XMVECTOR g_Position;
		XMMATRIX g_WorldMatrix;
	
	public:	

		virtual void Translate(DIRECTION, float) abstract;
		virtual void Rotate() abstract;
		virtual void Scale() abstract;
		XMMATRIX WorldMatrix() { return g_WorldMatrix; }
	};

}