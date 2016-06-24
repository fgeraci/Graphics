#pragma once

#include "pch.h"

using namespace Windows::System;
using namespace Windows::Foundation;

namespace Application {

	class MyApp sealed {

	private:

		Entity* g_SelectedEntity = nullptr;
		Gfx*	g_Graphics;
		Point	g_LastPointerData;
		
		bool g_Paused = false;

	public:

		bool IsPaused() {
			return g_Paused;
		}

		void SetPause(bool p) {
			g_Paused = p;
		}

		MyApp();

		// Application's main events
		void Update(Ticker^ t);
		void Resize(UINT, UINT);
		void Terminate();
		
	};
}