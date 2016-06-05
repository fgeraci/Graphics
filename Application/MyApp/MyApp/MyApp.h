#pragma once

#include "pch.h"
#include "Gfx.h"

namespace Application {

	ref class MyApp sealed {
	public:
		MyApp();
	private:
		Gfx^ m_Graphics;
		void Update();
	};

}