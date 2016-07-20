#pragma once

#include "pch.h"

namespace Application {
	
	namespace Interfaces {

		using namespace Application::Enums::Transformation;

		class ITransformable {

		public:

			virtual void Transform(TRANSFORMATION_TYPE, TRANSFORM_DIRECTION, TRANSFORM_HIERARCHY, float = 1.0f) = 0;

		protected:

			virtual void Translate(TRANSFORM_DIRECTION, TRANSFORM_HIERARCHY, float = 1.0f) = 0;
			virtual void Rotate(TRANSFORM_DIRECTION, TRANSFORM_HIERARCHY, float = 1.0f) = 0;
			virtual void Scale(TRANSFORM_DIRECTION, TRANSFORM_HIERARCHY, float = 1.0f) = 0;
		};

	}

}