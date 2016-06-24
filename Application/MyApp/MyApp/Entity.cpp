#pragma once

#include "pch.h"

using namespace Application;

Entity::Entity() {
	g_Position	= XMVectorZero();
	g_Up		= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_Right		= XMVectorSet(1.0, 0.0f, 0.0f, 0.0f);
	g_Forward	= XMVectorSet(0, 0, 1.0f, 0);
}

#pragma region Transformations

void Entity::Transform(TRANSFORMATION_TYPE t, DIRECTION d, float speed) {
	switch(t) {
	case TRANSLATE:
		Translate(d, speed);
		break;
	case ROTATE:
		Rotate(d,speed);
		break;
	}
	g_Dirty = false;
	UpdateWorldMatrix();
}

void Entity::UpdateWorldMatrix() {
	g_Forward = XMVector3Normalize(g_Forward);
	g_Up = XMVector3Normalize(XMVector3Cross(g_Forward, g_Right));
	g_Right = XMVector3Cross(g_Up, g_Forward);
}

void Entity::Translate(DIRECTION d, float speed) {
	// g_position += speed * forward
	XMVECTOR delta = XMVectorReplicate(speed);
	switch(d) {
	case WORLD_FORWARD: case WORLD_BACKWARDS:
		g_Position = XMVectorMultiplyAdd(delta, (d == WORLD_FORWARD ? 1.0f : -1.0f) * g_Forward, g_Position);
		break;
	case WORLD_STRAFE_LEFT: case WORLD_STRAFE_RIGHT:
		g_Position = XMVectorMultiplyAdd(delta, (d == WORLD_STRAFE_RIGHT ? 1.0f : -1.0f) * g_Right, g_Position);
		break;
	}
	g_Forward = XMVector3Normalize(g_Forward);
	g_Right = XMVector3Normalize(g_Right);
}

void Entity::Rotate(DIRECTION d, float speed = 1.0f) {
	// this function pointer will get the proper call
	int dir;
	XMVECTOR axis;
	switch(d) {
	case WORLD_RIGHT: case WORLD_LEFT: case LOCAL_RIGHT: case LOCAL_LEFT:
		dir				= (d == WORLD_RIGHT || d == LOCAL_RIGHT)  ? m_LeftScreenSide : m_RightScreenSide;
		axis			= (d == WORLD_RIGHT || d == WORLD_LEFT)	  ? m_WorldUpVector : g_Up;
		break;
	case WORLD_UP: case WORLD_DOWN: case LOCAL_UP: case LOCAL_DOWN:
		dir				= (d == WORLD_UP || d == LOCAL_UP) ? m_UpScreenSide : m_DownScreenSide;
		axis			= g_Right;
	}
	g_Up = XMVector3Normalize(Math::RotateOnAxis(g_Up, axis, dir,speed));
	g_Forward = XMVector3Normalize(Math::RotateOnAxis(g_Forward, axis, dir, speed));
	g_Right = XMVector3Normalize(Math::RotateOnAxis(g_Right, axis, dir, speed));
}

#pragma endregion Transformations