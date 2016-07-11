#pragma once

#include "pch.h"

using namespace Application;

void Entity::Translate(DIRECTION d, float speed) {
	// g_position += speed * forward
	XMVECTOR delta = XMVectorReplicate(speed);
	switch(d) {
	case WORLD_FORWARD: case WORLD_BACKWARDS:
		g_Position = XMVectorMultiplyAdd(delta, (d == WORLD_FORWARD ? 1 : -1) * g_Forward, g_Position);
		break;
	case WORLD_STRAFE_LEFT: case WORLD_STRAFE_RIGHT:
		g_Position = XMVectorMultiplyAdd(delta, (d == WORLD_STRAFE_RIGHT ? 1 : -1) * g_Right, g_Position);
		break;
	}
	
}