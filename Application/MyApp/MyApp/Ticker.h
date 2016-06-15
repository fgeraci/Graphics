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

	bool Ticker::Tick() {
		__int64 n = Now;
		__int64 delta = 0;
		delta = n - g_LastUpdate;
		bool tick = (delta > g_TargetTime) && !Paused;
		g_GlobalDelta += n - g_LastTickAttempt;
		if(tick) {
			g_CurrentFPS++;
			g_LastUpdate = n;
			g_DeltaTime = delta * (1.0 / g_Cps);	// in seconds, when was the last frame update
			if (g_GlobalDelta > g_Cps) {
				FPS = g_CurrentFPS;
				g_GlobalDelta = g_CurrentFPS = 0;
				PrintCurrentFPS();
			}
		}
		g_LastTickAttempt = n;
		return tick;
	}

	Ticker(int fps) {
		if (!fps) {
			throw Platform::Exception::CreateException(-1,"Invalid FPS parameter");
		}
		Paused = false;
		g_FPS = fps;
		g_CurrentFPS = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*)&g_Cps);
		g_TargetTime = g_Cps / g_FPS;
		QueryPerformanceCounter((LARGE_INTEGER*)&g_LastUpdate);
		g_LastTickAttempt = g_LastUpdate;
	}

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

	void PrintCurrentFPS() {
		std::wstring s;
		s += L"Current FPS: ";
		s += std::to_wstring(FPS);
		s += L"\n";
		OutputDebugString(s.c_str());
	}
};
