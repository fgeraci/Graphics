#pragma once

#include "pch.h"

namespace Application {
	
	enum DIRECTION {
		LOCAL_FORWARD,
		LOCAL_RIGHT,
		LOCAL_LEFT,
		LOCAL_BACKWARDS,
		LOCAL_DOWN,
		LOCAL_UP,
		LOCAL_STRAFE_RIGHT,
		LOCAL_STRAFE_LEFT,
		WORLD_FORWARD,
		WORLD_RIGHT,
		WORLD_LEFT,
		WORLD_BACKWARDS,
		WORLD_DOWN,
		WORLD_UP,
		WORLD_STRAFE_RIGHT,
		WORLD_STRAFE_LEFT
	};

	interface ITransformable {
		void Translate(DIRECTION,float);
		void Rotate();
		void Scale();
	};

}
