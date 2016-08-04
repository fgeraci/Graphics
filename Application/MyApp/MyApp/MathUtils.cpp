#pragma once

#include "pch.h"

using namespace DirectX;
using namespace Application::Math;

// Pre-compute the 1 degree rotation matrix in advance
// A single degree times modifier

float oneDeg = Application::m_RotationGlobalModifier * (XM_PI / 180.0f);
float cosOneDegPos = cos(oneDeg);
float sinOneDegPos = sin(oneDeg);

/* Misc */

float Application::Math::LinInt(float x, float x1, float x2, float y1, float y2) {
	return y1 + ((x - x1) * (y2 - y1)) / (x2 - x1);
}

float Application::Math::ToRadians(float angle) {
	return angle * (XM_PI / 180.0f);
}

float Application::Math::ToDegrees(float d) {
	return d * (180.0f / XM_PI);
}

/* XM methods */

XMFLOAT3 Application::Math::AddVectors3(XMFLOAT3 v1, XMFLOAT3 v2) {
	return XMFLOAT3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

XMMATRIX Application::Math::GetRotationMatrixForAxis(XMVECTOR v) {
	float x = XMVectorGetX(v), y = XMVectorGetY(v), z = XMVectorGetZ(v);
	float t = (1 - cosOneDegPos), c = cosOneDegPos, s = sinOneDegPos;
	return XMMatrixSet(
		c + t*(x*x), t*x*y + s*z, t*x*z - s*y, 0,
		t*x*y - s*z, c + t*(y*y), t*y*z + s*x, 0,
		t*x*z + s*y, t*y*z - s*x, c + t*(z*z), 0,
		0, 0, 0, 1
	);
}

XMMATRIX Application::Math::GetRotationMatrixForAxisWeighted(XMVECTOR v, float weight) {
	float x = XMVectorGetX(v) * weight, y = XMVectorGetY(v) * weight, z = XMVectorGetZ(v) * weight;
	float t = (1 - cosOneDegPos), c = cosOneDegPos, s = sinOneDegPos;
	return XMMatrixSet(
		c + t*(x*x), t*x*y + s*z, t*x*z - s*y, 0,
		t*x*y - s*z, c + t*(y*y), t*y*z + s*x, 0,
		t*x*z + s*y, t*y*z - s*x, c + t*(z*z), 0,
		0, 0, 0, 1
	);
}

XMMATRIX Application::Math::GetTranslationMatrix(XMVECTOR v, float speed) {
	return XMLoadFloat4x4(&GetTranslationMatrix4(v));
}

XMVECTOR Application::Math::RotateOnAxis(const XMVECTOR& v, XMVECTOR rotAxis, int dir) {
	XMMATRIX m;
	bool clock = (dir == 1);
	m = clock ? GetRotationMatrixForAxis(rotAxis) : XMMatrixTranspose(GetRotationMatrixForAxis(rotAxis));
	// remember that the inverse of an orthogonal matrix is its tranposed one!
	return XMVector3TransformNormal(v, m);
}

float Application::Math::VectorMagnitude(XMVECTOR v) {
	return
		sqrt(pow(XMVectorGetX(v), 2) + pow(XMVectorGetY(v), 2) + pow(XMVectorGetZ(v), 2));
}

/* FLOATx methods */

XMFLOAT4X4 Application::Math::MatrixIdentity() {
	XMFLOAT4X4 m = { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
	return m;
}

XMFLOAT4 Application::Math::VectorAdd(const XMFLOAT4& v1, const XMFLOAT4& v2) {
	return XMFLOAT4(v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z, v1.w);
}

XMFLOAT4 Application::Math::Normalize(const XMFLOAT4& v) {
	float length = sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
	return XMFLOAT4((v.x / length), (v.y / length), (v.z / length), v.w);
}

XMFLOAT4 Application::Math::Transform4(const XMFLOAT4& v, XMFLOAT4X4 m) {
	return XMFLOAT4(
		(v.x * m._11 + v.y * m._21 + v.z * m._31 + v.w * m._41),
		(v.x * m._12 + v.y * m._22 + v.z * m._32 + v.w * m._42),
		(v.x * m._13 + v.y * m._23 + v.z * m._33 + v.w * m._43),
		v.w);
}

XMFLOAT3 Application::Math::Transform3(const XMFLOAT3& v, XMFLOAT4X4 m) {
	XMFLOAT4 vec = { v.x, v.y, v.z, 1.0f };
	return XMFLOAT3(
		(vec.x * m._11 + vec.y * m._21 + vec.z * m._31 + vec.w * m._41),
		(vec.x * m._12 + vec.y * m._22 + vec.z * m._32 + vec.w * m._42),
		(vec.x * m._13 + vec.y * m._23 + vec.z * m._33 + vec.w * m._43));
}

float Application::Math::DotProduct4(const XMFLOAT4 v1, const XMFLOAT4 v2) {
	return (v1.x*v2.x
		+ v1.y*v2.y
		+ v1.z*v2.z);
}

void Application::Math::MultiplyAdd(float d, XMVECTOR v1, XMFLOAT3& v2) {
	float x = XMVectorGetX(v1), y = XMVectorGetY(v1), z = XMVectorGetZ(v1);
	v2 = XMFLOAT3(
		(x*d) + v2.x, (y*d) + v2.y, (z*d) + v2.z);
}

void Application::Math::RotateOnAxis(XMFLOAT3& v, XMFLOAT4X4 rotMat, int dir) {
	v = XMFLOAT3(rotMat._11*v.x + rotMat._12*v.y + rotMat._13*v.z,
		rotMat._21*v.x + rotMat._22*v.y + rotMat._23*v.z,
		rotMat._31*v.x + rotMat._32*v.y + rotMat._33*v.z);
}

XMFLOAT4X4 Application::Math::GetRotationMatrixForAxis4(XMVECTOR a) {
	XMFLOAT4 v;
	XMStoreFloat4(&v, a);
	float x = v.x, y = v.y, z = v.z;
	float t = (1 - cosOneDegPos), c = cosOneDegPos, s = sinOneDegPos;
	return XMFLOAT4X4(
		c + t*(x*x), t*x*y + s*z, t*x*z - s*y, 0,
		t*x*y - s*z, c + t*(y*y), t*y*z + s*x, 0,
		t*x*z + s*y, t*y*z - s*x, c + t*(z*z), 0,
		0, 0, 0, 1
	);
}

XMFLOAT4X4 Application::Math::GetTranslationMatrix4(XMVECTOR a, float speed) {
	XMFLOAT4 v;
	XMStoreFloat4(&v, a);
	float x = speed * v.x, y = speed * v.y, z = speed * v.z;
	XMFLOAT4X4 m = Math::MatrixIdentity();
	m._41 = x;
	m._42 = y;
	m._43 = z;
	m._44 = 1.0f;
	return m;
}
	
