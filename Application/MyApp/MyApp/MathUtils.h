#pragma once

#include "pch.h"

using namespace DirectX;

namespace Math {
	
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
}

