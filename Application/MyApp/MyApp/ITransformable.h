#pragma once

#include "pch.h"

namespace Application {
	
	enum TRANSFORMATION_TYPE {
		TRANSLATE,
		ROTATE,
		SCALE
	};

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

	class ITransformable {

	public:

		virtual void Transform(TRANSFORMATION_TYPE, DIRECTION, float speed = 1.0f) = 0;

	protected:

		virtual void Translate(DIRECTION,float) = 0;
		virtual void Rotate(DIRECTION,float) = 0;
		virtual void Scale() = 0;
	};

}