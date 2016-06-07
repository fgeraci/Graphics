#pragma once

#include "pch.h"
#include "Gfx.h"

using namespace Windows::UI::Core;

namespace Application {

	ref class MyApp sealed {

		~MyApp() {
			delete g_Graphics;
		}

	public:

		MyApp();
		void Update();
		void Resize(UINT,UINT);
		void Terminate();

	private:

		Gfx^ g_Graphics;

	};

}