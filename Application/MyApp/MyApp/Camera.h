#pragma once

#include "pch.h"

using namespace DirectX;

namespace Application {

	class Camera : public Entity {
	
	private:
		
		bool g_IsPrimary;
		XMMATRIX g_WorldViewProj;
		XMMATRIX g_WorldView;
		
	public:
		
		Camera() : Entity() {
			g_Position			= XMVectorSet(0.0f, 3.0f, -10.0f, 1.0f);
			XMVECTOR g_Target	= XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			g_Forward			= XMVector3Normalize(XMVectorSubtract(g_Target, g_Position));
			g_Dirty				= true;
		}

		Camera(bool isPrimary) : Camera() {
			g_IsPrimary = isPrimary;
		}

		Camera(bool isPrimary, XMFLOAT4 initPosition) : Camera(isPrimary) {
			g_IsPrimary = isPrimary;
			g_Position = XMLoadFloat4(&initPosition);
		}

		void UpdateWorldMatrix() override {
			Entity::UpdateWorldMatrix();
			// Camera is always dirty for update
			g_Dirty = true;
			// Update also view matrix
		}

		XMVECTOR Target()		{ return XMVectorAdd(g_Position, g_Forward); }

		bool IsPrimary()		{ return g_IsPrimary; }

		XMMATRIX WorldViewProject() {
			return g_WorldViewProj;
		}

		void SetWorldViewProject(XMMATRIX m) {
			g_WorldViewProj = m;
		}

		void Camera::Rotate(DIRECTION d, float speed) {
			// intercept direction and always make it yaw world
			if (d == LOCAL_LEFT) d = WORLD_LEFT;
			if (d == LOCAL_RIGHT) d = WORLD_RIGHT;
			Entity::Rotate(d,speed);
		}


		void Scale() override { return; }
	};
}