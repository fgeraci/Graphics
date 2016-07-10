#pragma once

#include "pch.h"

using namespace DirectX;
using namespace Math;

namespace Application {

	class Camera : public Entity {
	
	private:
		
		bool g_IsPrimary;
		XMFLOAT4 g_Target;
		
	public:
		
		Camera() {
			g_Position		= DirectX::XMFLOAT4(0.0f, 3.0f, -10.0f, 1.0f);
			g_Up			= DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
			g_Target		= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			g_Right			= XMFLOAT4(1.0, 0.0f , 0.0f , 0.0f);
			g_Forward		= XMFLOAT4(	g_Position.x - g_Target.x ,
										g_Position.y - g_Target.y , g_Position.z - g_Target.z, 0.0f);
			g_WorldMatrix	= MatrixIdentity();
		}

		Camera(bool isPrimary) : Camera() {
			g_IsPrimary = isPrimary;
		}

		Camera(bool isPrimary, XMFLOAT4 initPosition) : Camera(isPrimary) {
			g_IsPrimary = isPrimary;
			g_Position = initPosition;
		}

		XMFLOAT4 Position() { return g_Position; }
		XMFLOAT4 Target()	{ return g_Target; }
		XMFLOAT4 Up()		{ return g_Up; }
		bool IsPrimary()	{ return g_IsPrimary; }

		void Translate(DIRECTION dir, float speed) {
			switch (dir) {
			case LOCAL_FORWARD:
				break;
			}
		}

		void Rotate()	 { }
		void Scale()	 { }
	};
}
