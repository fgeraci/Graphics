#pragma once

#include "pch.h"

using namespace Windows::System;
using namespace Windows::Foundation;

namespace Application {

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

	class MyApp sealed {

	private:

		Entity* g_SelectedEntity = nullptr;
		Gfx*	g_Graphics;
		Point	g_LastPointerData;
		
		bool g_Paused = false;
		std::array<VirtualKey, INPUT_MAP_UI::INPUT_MAP_SIZE_UI> g_InputMapUI;
		std::array<VirtualKey, INPUT_MAP_ACTION::INPUT_MAP_SIZE_ACTION> g_InputMapAction;
		std::array<VirtualKey, INPUT_MAP_POINTER::INPUT_MAP_SIZE_POINTER> g_InputMapPointer;
		void InitializeInput();
		
		// Keyboard will be managed asynchronously from within the App
		void ProcessInput(INPUT_MAP_ACTION, double);
		void ReadAsyncKeys();
		bool IsKeyDown(VirtualKey);

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

		// Mouse will be managed by the UWP framework
		void ProcessPointer(Point, VirtualKey, POINT_EVENT_TYPE, int = 0);
		
	};
}