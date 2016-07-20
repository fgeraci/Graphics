#pragma once

#include "pch.h"

using namespace Platform;

namespace Application {

	class Ticker sealed {

	public:

		static double DeltaTime;

		bool Paused() { return g_Paused; }

		int FPS() { return g_CurrentFPS; }

		bool Tick();

		Ticker(int);

	private:

		bool g_Paused = false;

		__int64 g_Cps;
		__int64 g_LastUpdate;
		__int64 g_GlobalDelta;
		__int64 g_LastTickAttempt;
		int g_CurrentFPS;
		__int64 g_TargetTime;
	

		UINT g_FPS = 0;

		 __int64 Now() {
			__int64 time;
			QueryPerformanceCounter((LARGE_INTEGER*)&time);
			return time;
		}

		void PrintCurrentFPS();
	};
}