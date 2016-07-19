#pragma once

#include "pch.h"

using namespace Application;

Ticker::Ticker(int fps) {
	if (!fps) {
		throw Platform::Exception::CreateException(-1, "Invalid FPS parameter");
	}
	Paused = false;
	g_FPS = fps;
	g_CurrentFPS = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&g_Cps);
	g_TargetTime = g_Cps / g_FPS;
	QueryPerformanceCounter((LARGE_INTEGER*)&g_LastUpdate);
	g_LastTickAttempt = g_LastUpdate;
}

bool Ticker::Tick() {
	__int64 n = Now;
	__int64 delta = 0;
	delta = n - g_LastUpdate;
	bool tick = (delta > g_TargetTime) && !Paused;
	g_GlobalDelta += n - g_LastTickAttempt;
	if (tick) {
		g_CurrentFPS++;
		g_LastUpdate = n;
		g_DeltaTime = delta * (1.0 / g_Cps);	// in seconds, when was the last frame update
		if (g_GlobalDelta > g_Cps) {
			FPS = g_CurrentFPS;
			g_GlobalDelta = g_CurrentFPS = 0;
#if defined (_DEBUG)
			// PrintCurrentFPS();
#endif
		}
	}
	g_LastTickAttempt = n;
	return tick;
}

void Ticker::PrintCurrentFPS() {
	std::wstring s;
	s += L"Current FPS: ";
	s += std::to_wstring(FPS);
	s += L"\n";
	LPCWSTR a = s.c_str();
	LOGMESSAGE(a);
}