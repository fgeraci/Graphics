#pragma once

#include "pch.h"

using namespace DirectX;

namespace Application {
	
	class WorldEntity sealed : public Entity {
	
	public:
		
		WorldEntity() {
			g_Position  = XMVectorSet(	0,	 0,		0,	1.0f);
			g_Right		= XMVectorSet( 1.0f, 0,		0,	0);
			g_Up		= XMVectorSet( 0,	1.0f,	0,  0);
			g_Forward	= XMVectorSet( 0,	 0,	  1.0f, 0);
		}

		void Transform(TRANSFORMATION_TYPE, TRANSFORM_DIRECTION, TRANSFORM_HIERARCHY, float) override { }
		void Translate(TRANSFORM_DIRECTION, TRANSFORM_HIERARCHY, float) override { }
		void Rotate(TRANSFORM_DIRECTION, TRANSFORM_HIERARCHY, float) override { }
		void Scale(TRANSFORM_DIRECTION, TRANSFORM_HIERARCHY, float) override { }
	};

}
