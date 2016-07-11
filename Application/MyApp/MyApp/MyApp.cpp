#pragma once

#include "pch.h"


using namespace Application;
using namespace Windows::UI::Core;

MyApp::MyApp() {
	g_Graphics = new Gfx();
	InitializeInput();
	g_SelectedEntity = g_Graphics->MainCamera();
}

void MyApp::Update(Ticker^ t) {
	
	if(t->Tick()) {
		// process keyboard input
		ReadAsyncKeys(t->DeltaTime);
		// render
		g_Graphics->Draw();
	}

}

void MyApp::ReadAsyncKeys(double delta) {
	for (int i = 0; i < g_InputMap.size(); ++i) {
		if (CoreWindow::GetForCurrentThread()->GetAsyncKeyState(g_InputMap[i]) == CoreVirtualKeyStates::Down) {
			ProcessKeyboard((INPUT_MAP)i, delta);
		}
	}
}

void MyApp::Resize(UINT width, UINT height) {
	g_Graphics->ClientHeight(height);
	g_Graphics->ClientWidht(width);
	g_Graphics->ResizeBuffers();
}

void MyApp::InitializeInput() {
	g_InputMap[OPTIONS] = VirtualKey::O;
	g_InputMap[FORWARD] = VirtualKey::W;
	g_InputMap[BACKWARDS] = VirtualKey::S;
	g_InputMap[LEFT] = VirtualKey::Q;
	g_InputMap[RIGHT] = VirtualKey::E;
	g_InputMap[LEFT_STRAFE] = VirtualKey::A;
	g_InputMap[RIGHT_STRAFE] = VirtualKey::D;
	g_InputMap[ESCAPE] = VirtualKey::Escape;
	g_InputMap[PAUSE] = VirtualKey::P;

	for (int i = 0; i < g_InputFlags.size(); ++i) g_InputFlags[i] = false;
}

void MyApp::FlagInput(INPUT_MAP key) {
	g_InputFlags[key] = true;
}

void MyApp::Terminate() {
	g_Graphics->Terminate();
}

void MyApp::ProcessKeyboard(INPUT_MAP key, double delta) {
	delta *= g_SpeedModifier;
	switch(key) {
	case FORWARD: 
		g_SelectedEntity->Translate(DIRECTION::WORLD_FORWARD, static_cast<float>(delta));
		break;
	case BACKWARDS:
		g_SelectedEntity->Translate(DIRECTION::WORLD_BACKWARDS, static_cast<float>(delta));
		break;
	case LEFT_STRAFE:
		g_SelectedEntity->Translate(DIRECTION::WORLD_STRAFE_LEFT, static_cast<float>(delta));
		break;
	case RIGHT_STRAFE:
		g_SelectedEntity->Translate(DIRECTION::WORLD_STRAFE_RIGHT, static_cast<float>(delta));
		break;
	}
}