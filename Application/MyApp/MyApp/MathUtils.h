#pragma once

#include "pch.h"

using namespace DirectX;

namespace Application {
	
	namespace Math {
		
		// Pre-compute the 1 degree rotation matrix in advance
		// A single degree times modifier
		static float oneDeg = Application::m_RotationGlobalModifier * (XM_PI / 180.0f);
		static float cosOneDegPos = cos(oneDeg);
		static float sinOneDegPos = sin(oneDeg);

		/* XM methods */

		static XMMATRIX GetRotationMatrixForAxis(XMVECTOR v, float speed = 1.0f) {
			float x = speed * XMVectorGetX(v), y = speed * XMVectorGetY(v), z = speed * XMVectorGetZ(v);
			float t = (1 - cosOneDegPos), c = cosOneDegPos, s = sinOneDegPos;
			return XMMatrixSet(
							c + t*(x*x), t*x*y + s*z, t*x*z - s*y, 0,
							t*x*y - s*z, c + t*(y*y), t*y*z + s*x, 0,
							t*x*z + s*y, t*y*z - s*x, c + t*(z*z), 0,
							0, 0, 0, 1
			);
		}

		inline XMVECTOR RotateOnAxis (const XMVECTOR& v, XMVECTOR rotAxis, int dir = 1, float speed = 1.0f) {
			XMMATRIX m;
			bool clock = (dir == 1);
			m = clock ? GetRotationMatrixForAxis(rotAxis, speed) : XMMatrixTranspose(GetRotationMatrixForAxis(rotAxis,speed));
			// remember that the inverse of an orthogonal matrix is its tranposed one!
			return XMVector3TransformNormal(v, m);
		}

		inline float VectorMagnitude(XMVECTOR v) {
			return
				sqrt(pow(XMVectorGetX(v), 2) + pow(XMVectorGetY(v), 2) + pow(XMVectorGetZ(v), 2));
		}

		/* FLOATx methods */

		inline XMFLOAT4X4 MatrixIdentity() {
			XMFLOAT4X4 m = {	1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f, 
								0.0f, 0.0f, 1.0f, 0.0f, 
								0.0f, 0.0f, 0.0f, 1.0f	};
			return m;
		}

		inline XMFLOAT4 VectorAdd(const XMFLOAT4& v1, const XMFLOAT4& v2) {
			return XMFLOAT4(	v1.x + v2.x , 
								v1.y + v2.y , 
								v1.z + v2.z , v1.w);
		}

		inline XMFLOAT4 Normalize(const XMFLOAT4& v) {
			float length = sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
			return XMFLOAT4((v.x / length), (v.y / length), (v.z / length), v.w);
		}

		inline XMFLOAT4 Transform4(const XMFLOAT4& v, XMFLOAT4X4 m) {
			return XMFLOAT4(
				(v.x * m._11 + v.y * m._21 + v.z * m._31 + v.w * m._41),
				(v.x * m._12 + v.y * m._22 + v.z * m._32 + v.w * m._42),
				(v.x * m._13 + v.y * m._23 + v.z * m._33 + v.w * m._43),
				v.w);
		}

		inline float DotProduct4(const XMFLOAT4 v1, const XMFLOAT4 v2) {
			return (v1.x*v2.x 
						+ v1.y*v2.y 
							+ v1.z*v2.z);
		}

		inline void MultiplyAdd(float d, XMVECTOR v1, XMFLOAT3& v2) {
			float x = XMVectorGetX(v1), y = XMVectorGetY(v1), z = XMVectorGetZ(v1);
			v2 = XMFLOAT3 (
				(x*d) + v2.x, (y*d) + v2.y, (z*d) + v2.z);
		}

		inline void RotateOnAxis(XMFLOAT3& v, XMFLOAT4X4 rotMat, int dir = 1, float speed = 1.0f) {
			v = XMFLOAT3(	rotMat._11*v.x + rotMat._12*v.y + rotMat._13*v.z,
							rotMat._21*v.x + rotMat._22*v.y + rotMat._23*v.z,
							rotMat._31*v.x + rotMat._32*v.y + rotMat._33*v.z);
		}

		static XMFLOAT4X4 GetRotationMatrixForAxis4(XMVECTOR a, float speed = 1.0f) {
			XMFLOAT4 v;
			XMStoreFloat4(&v, a);
			float x = speed * v.x, y = speed * v.y, z = speed * v.z;
			float t = (1 - cosOneDegPos), c = cosOneDegPos, s = sinOneDegPos;
			return XMFLOAT4X4(
				c + t*(x*x), t*x*y + s*z, t*x*z - s*y, 0,
				t*x*y - s*z, c + t*(y*y), t*y*z + s*x, 0,
				t*x*z + s*y, t*y*z - s*x, c + t*(z*z), 0,
				0, 0, 0, 1
			);
		}
	}
}

