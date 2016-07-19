#pragma once

#include "pch.h"

using namespace DirectX;

namespace Application {
	
	namespace Math {
		
		/* XM methods */

		XMMATRIX GetRotationMatrixForAxis(XMVECTOR v);

		XMMATRIX GetRotationMatrixForAxisWeighted(XMVECTOR v, float weight = 1.0f);

		XMMATRIX GetTranslationMatrix(XMVECTOR v, float speed = 1.0f);

		XMVECTOR RotateOnAxis(const XMVECTOR& v, XMVECTOR rotAxis, int dir = 1);

		float VectorMagnitude(XMVECTOR v);

		/* FLOATx methods */

		XMFLOAT4X4 MatrixIdentity();

		XMFLOAT4 VectorAdd(const XMFLOAT4& v1, const XMFLOAT4& v2);

		XMFLOAT4 Normalize(const XMFLOAT4& v);

		XMFLOAT4 Transform4(const XMFLOAT4& v, XMFLOAT4X4 m);

		XMFLOAT3 Transform3(const XMFLOAT3& v, XMFLOAT4X4 m);

		float DotProduct4(const XMFLOAT4 v1, const XMFLOAT4 v2);

		void MultiplyAdd(float d, XMVECTOR v1, XMFLOAT3& v2);

		void RotateOnAxis(XMFLOAT3& v, XMFLOAT4X4 rotMat, int dir = 1);

		XMFLOAT4X4 GetRotationMatrixForAxis4(XMVECTOR a);

		XMFLOAT4X4 GetTranslationMatrix4(XMVECTOR a, float speed = 1.0f);
	}
}

