#pragma once

#include "pch.h"

using namespace DirectX;
using namespace Math;

namespace Application {

	class Camera : public Entity {
	
	private:
		
		bool g_IsPrimary;
		XMVECTOR g_Target;
		
	public:
		
		Camera() {
			g_Position		= XMVectorSet(0.0f, 3.0f, -10.0f, 1.0f);
			g_Up			= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			g_Target		= XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			g_Right			= XMVectorSet(1.0, 0.0f , 0.0f , 0.0f);
			g_Forward		= XMVector3Normalize(XMVectorSubtract(g_Target, g_Position));
			g_WorldMatrix	= XMMatrixIdentity();
		}

		Camera(bool isPrimary) : Camera() {
			g_IsPrimary = isPrimary;
		}

		Camera(bool isPrimary, XMFLOAT4 initPosition) : Camera(isPrimary) {
			g_IsPrimary = isPrimary;
			g_Position = XMLoadFloat4(&initPosition);
		}

		XMVECTOR Position() { return g_Position; }
		XMVECTOR Target()	{ return g_Target; }
		XMVECTOR Up()		{ return g_Up; }
		bool IsPrimary()	{ return g_IsPrimary; }

		void Translate(DIRECTION d, float f) override { 
			// camera need to update target
			XMVECTOR oldPos = 1 * g_Position;
			Entity::Translate(d, f); 
			g_Target += XMVectorSubtract(g_Position, oldPos);
		}

		void Rotate() override { }
		void Scale() override { }
	};
}
