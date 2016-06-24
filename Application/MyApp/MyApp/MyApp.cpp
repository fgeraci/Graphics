#pragma once

#include "pch.h"


using namespace Application;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;

MyApp::MyApp() {
	g_Paused = false;
	g_Graphics = new Gfx();
	g_SelectedEntity = g_Graphics->MainCamera();
}

void MyApp::Update(Ticker^ t) {

	if(t->Tick()) {
		// render
		g_Graphics->Draw();
	}

}

void MyApp::Resize(UINT width, UINT height) {
	g_Graphics->ClientHeight(height);
	g_Graphics->ClientWidht(width);
	g_Graphics->ResizeBuffers();
}

void MyApp::Terminate() {
	g_Graphics->Terminate();
}

#pragma region I/O


#pragma endregion I/O