#pragma once

#include "pch.h"

namespace Application {
	
	class Entity : public ITransformable {
	
	protected:
	
		XMFLOAT4 g_Up;
		XMFLOAT4 g_Forward;
		XMFLOAT4 g_Right;
		XMFLOAT4 g_Position;
		XMFLOAT4X4 g_WorldMatrix;
	
	public:	

		virtual void Translate(DIRECTION, float) abstract;
		virtual void Rotate()  abstract;
		virtual void Scale()  abstract;
		XMFLOAT4X4 WorldMatrix() { return g_WorldMatrix; }
	};

}