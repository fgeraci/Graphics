#pragma once

#include "pch.h"

using namespace DirectX;
using namespace Application::Math;
using namespace Application::Enums::Transformation;

namespace Application {

	class Camera : public Entity {
	
	private:
		
		std::unique_ptr<UploadBuffer<FrameConstantData>> g_UploadBuffer = nullptr;
		bool g_IsPrimary;
		
		XMMATRIX g_View;
		XMMATRIX g_Projection;
		XMMATRIX g_WorldView;
		XMMATRIX g_ViewProj;
		
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

		void InitializeUploadBuffer(ComPtr<ID3D12Device> dev) override {
			g_UploadBuffer = std::make_unique<UploadBuffer<FrameConstantData>>(dev, 1, true);
			InitConstantBuffer(dev);
			g_CPUVertexBuffer = g_UploadBuffer->GetResource();
		}

		UploadBuffer<FrameConstantData>* GetUploadBuffer() {
			return g_UploadBuffer.get();
		}

		void UpdateWorldMatrix() override {
			Entity::UpdateWorldMatrix();
			// Camera is always dirty for update
			g_Dirty = true;
			// Update also view matrix
		}

		XMVECTOR Target()		{ return XMVectorAdd(g_Position, g_Forward); }

		bool IsPrimary()		{ return g_IsPrimary; }


		XMMATRIX View() {
			return g_View;
		}
		
		XMMATRIX Projection() {
			return g_Projection;
		}

		XMMATRIX ViewProject() {
			return g_ViewProj;
		}

		void SetView(XMMATRIX m) {
			g_View = m;
		}

		void SetProjection(XMMATRIX m) {
			g_Projection = m;
		}

		void SetViewProject(XMMATRIX m) {
			g_ViewProj = m;
		}

		void Camera::Rotate(TRANSFORM_DIRECTION d, TRANSFORM_HIERARCHY h, float angle) override {
			float dir = m_LeftScreenSide;
			switch (d) {
			case TRANSFORM_DIRECTION::LEFT: 
			case TRANSFORM_DIRECTION::UP: 
			case TRANSFORM_DIRECTION::STRAFE_LEFT:
				dir = m_RightScreenSide;
			}
			XMVECTOR axis;
			switch (d) {
			case TRANSFORM_DIRECTION::RIGHT: 
			case TRANSFORM_DIRECTION::LEFT:
				axis = m_WorldUpVector;
				break;
			case TRANSFORM_DIRECTION::STRAFE_RIGHT: 
			case TRANSFORM_DIRECTION::STRAFE_LEFT:
				axis = g_Forward;
				break;
			case TRANSFORM_DIRECTION::UP: 
			case TRANSFORM_DIRECTION::DOWN:
				axis = g_Right;
			}
			
			XMMATRIX r = (dir == 1) ? 
				Math::GetRotationMatrixForAxisWeighted(axis, angle) : 
				XMMatrixTranspose(Math::GetRotationMatrixForAxisWeighted(axis, angle));
			
			g_Up = XMVector3Normalize(XMVector4Transform(g_Up, r));
			g_Forward = XMVector3Normalize(XMVector4Transform(g_Forward, r));
			g_Right = XMVector3Normalize(XMVector4Transform(g_Right, r));
		}

		/* Do nothing - who would scale a camera! */
		void Scale(TRANSFORM_DIRECTION d, TRANSFORM_HIERARCHY h, float angle) override { return; }
	};
}