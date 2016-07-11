#pragma once

#include "pch.h"

using namespace Windows::System;

namespace Application {

	enum INPUT_MAP {
		FIRST_KEY = 0,
		OPTIONS,
		FORWARD,
		BACKWARDS,
		LEFT_STRAFE,
		RIGHT_STRAFE,
		LEFT,
		RIGHT,
		ESCAPE,
		PAUSE,
		INPUT_MAP_SIZE
	};

	class MyApp sealed {

	public:

		int GlobalFPS() {
			return g_GlobalFPS;
		}

		std::array<VirtualKey, INPUT_MAP::INPUT_MAP_SIZE> InputMap() {
			return g_InputMap;
		}

		MyApp();

		void Update(Ticker^ t);
		void Resize(UINT,UINT);
		void Terminate();
		void FlagInput(INPUT_MAP);

	private:

		Entity* g_SelectedEntity = nullptr;
		Gfx* g_Graphics;
		int g_GlobalFPS = 60;
		std::array<VirtualKey, INPUT_MAP::INPUT_MAP_SIZE> g_InputMap;
		std::array<bool, INPUT_MAP::INPUT_MAP_SIZE> g_InputFlags;
		int g_SpeedModifier = 10;
		void InitializeInput();
		void ProcessKeyboard(INPUT_MAP,double);
		void ReadAsyncKeys(double);

	};
}