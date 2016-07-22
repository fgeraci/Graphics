#pragma once

#include "pch.h"

using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Application::Enums::Input;

namespace Application {

	class MyApp sealed {

	private:

		Entity* g_SelectedEntity = nullptr;
		std::vector<Entity*> g_Entities;
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
		void UpdateLogic();

	public:

		bool IsPaused() {
			return g_Paused;
		}

		void SetPause(bool p) {
			g_Paused = p;
		}

		MyApp();

		// Application's main events
		void Update(Ticker* t);
		void Resize(UINT, UINT);
		void Terminate();

		// Mouse will be managed by the UWP framework
		void ProcessPointer(Point, VirtualKey, POINT_EVENT_TYPE, int = 0);
		
	};
}