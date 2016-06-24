#pragma once

#include "pch.h"

using namespace DirectX;

namespace Application {

	namespace Math {
	
		// TODO - remove this 0.01f speed modifier from here. 
		// Although this enable us to pre-compute the 1 degree rotation matrix in advance
		
		static float oneDeg = Application::m_GlobalSpeedModifier * (XM_PI / 180.0f);
		static float cosOneDegPos = cos(oneDeg);
		static float sinOneDegPos = sin(oneDeg);

		static XMMATRIX GetRotationMatrixForAxis(XMVECTOR v, float speed = 1) {
			float x = speed * XMVectorGetX(v), y = speed * XMVectorGetY(v), z = speed * XMVectorGetZ(v);
			float t = (1 - cosOneDegPos), c = cosOneDegPos, s = sinOneDegPos;
			return XMMatrixSet(
							c + t*(x*x), t*x*y + s*z, t*x*z - s*y, 0,
							t*x*y - s*z, c + t*(y*y), t*y*z + s*x, 0,
							t*x*z + s*y, t*y*z - s*x, c + t*(z*z), 0,
							0, 0, 0, 1
			);
		}

		static XMMATRIX MatrixRotationYLeft = GetRotationMatrixForAxis(XMVectorSet(0, 1, 0, 0));

		static XMMATRIX MatrixRotationYRight = XMMatrixTranspose(MatrixRotationYLeft);

		inline XMVECTOR RotateOnAxis (const XMVECTOR& v, XMVECTOR rotAxis, int dir = 1, float speed = 1.0f) {
			XMMATRIX m;
			bool clock = (dir == 1);
			m = clock ? GetRotationMatrixForAxis(rotAxis, speed) : XMMatrixTranspose(GetRotationMatrixForAxis(rotAxis,speed));
			// remember that the inverse of an orthogonal matrix is its tranposed one!
			return XMVector3TransformNormal(v, m);
		}

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

		inline float Dot(const XMFLOAT4 v1, const XMFLOAT4 v2) {
			return (v1.x*v2.x 
						+ v1.y*v2.y 
							+ v1.z*v2.z);
		}

		inline float VectorMagnitude(XMVECTOR v) {
			return
				sqrt(pow(XMVectorGetX(v), 2) + pow(XMVectorGetY(v), 2) + pow(XMVectorGetZ(v), 2));
		}
	}
}

