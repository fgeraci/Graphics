#pragma once

#include "pch.h"

using namespace Platform;

ref class Ticker sealed {

public:

	property bool Paused;

	property double DeltaTime {
		double get() {
			return g_DeltaTime;
		}
	}

	property int FPS;

	bool Tick();

	Ticker(int);

private:

	__int64 g_Cps;
	__int64 g_LastUpdate;
	__int64 g_GlobalDelta;
	__int64 g_LastTickAttempt;
	int g_CurrentFPS;
	double g_DeltaTime;
	__int64 g_TargetTime;
	

	UINT g_FPS = 0;

	property __int64 Now {
		__int64 get() {
			__int64 time;
			QueryPerformanceCounter((LARGE_INTEGER*)&time);
			return time;
		}
	}

	void PrintCurrentFPS();
};
