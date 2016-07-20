#pragma once

#include "pch.h"

namespace Application {

	namespace Enums {


		namespace Entity {

			enum ENTITY_TYPE {
				POLYGON
			};

		}

		namespace Transformation {

			enum TRANSFORMATION_TYPE {
				TRANSLATE,
				ROTATE,
				SCALE
			};

			enum TRANSFORM_HIERARCHY {
				WORLD,
				PARENT,
				LOCAL
			};

			enum TRANSFORM_DIRECTION {
				FORWARD,
				RIGHT,
				LEFT,
				STRAFE_LEFT,
				STRAFE_RIGHT,
				BACKWARDS,
				UP,
				DOWN
			};

		}

		namespace Input {

			enum INPUT_MAP_UI {
				OPTIONS = 0,
				ESCAPE,
				PAUSE,
				INPUT_MAP_SIZE_UI
			};

			enum INPUT_MAP_POINTER {
				CLICK_ACTION = 0,
				CLICK_SELECT,
				CLICK_DRAG_ACTION,
				CLICK_DRAG_SELECT,
				INPUT_MAP_SIZE_POINTER
			};

			enum INPUT_MAP_ACTION {
				FORWARD = 0,
				BACKWARDS,
				LEFT_STRAFE,
				RIGHT_STRAFE,
				LEFT,
				RIGHT,
				MODIFIER,
				MODIFIER_SEC,
				INPUT_MAP_SIZE_ACTION
			};

			enum POINT_EVENT_TYPE {
				DOWN,
				WHEEL,
				DRAG
			};

		}
	}
}