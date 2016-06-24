#pragma once

#include "pch.h"

namespace Application {

	ref class MyApp sealed {

	public:

		property int GlobalFPS {
			int get() {
				return g_GlobalFPS;
			}
		}

		MyApp();
		void Update();
		void Resize(UINT,UINT);
		void Terminate();
		

	private:

		Gfx^ g_Graphics;
		int g_GlobalFPS = 30;

	};

}