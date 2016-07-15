#pragma once

#include "pch.h"


using namespace Application;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;

MyApp::MyApp() {
	g_Paused = false;
	g_Graphics = new Gfx();
	InitializeInput();
	g_SelectedEntity = g_Graphics->MainCamera();
}

void MyApp::Update(Ticker^ t) {
	
	// process keyboard input
	if(t->Tick()) {
		ReadAsyncKeys();
		// render
		g_Graphics->Draw();
	}

}

void MyApp::ReadAsyncKeys() {
	for (int i = 0; i < g_InputMapAction.size(); ++i) {
		if (IsKeyDown(g_InputMapAction[i])) {
			ProcessInput((INPUT_MAP_ACTION)i, m_GlobalSpeedModifier);
		}
	}
}

void MyApp::Resize(UINT width, UINT height) {
	g_Graphics->ClientHeight(height);
	g_Graphics->ClientWidht(width);
	g_Graphics->ResizeBuffers();
}

void MyApp::InitializeInput() {
	// UI map
	g_InputMapUI[OPTIONS] = VirtualKey::O;
	g_InputMapUI[ESCAPE] = VirtualKey::Escape;
	g_InputMapUI[PAUSE] = VirtualKey::P;
	// Action map
	g_InputMapAction[FORWARD] = VirtualKey::W;
	g_InputMapAction[BACKWARDS] = VirtualKey::S;
	g_InputMapAction[LEFT] = VirtualKey::Q;
	g_InputMapAction[RIGHT] = VirtualKey::E;
	g_InputMapAction[LEFT_STRAFE] = VirtualKey::A;
	g_InputMapAction[RIGHT_STRAFE] = VirtualKey::D;
	g_InputMapAction[MODIFIER] = VirtualKey::LeftShift;
	g_InputMapAction[MODIFIER_SEC] = VirtualKey::LeftControl;
	// Pointer map
	g_InputMapPointer[CLICK_ACTION] = VirtualKey::RightButton;
	g_InputMapPointer[CLICK_SELECT] = VirtualKey::LeftButton;
	g_InputMapPointer[CLICK_DRAG_ACTION] = VirtualKey::RightButton;
}

void MyApp::Terminate() {
	g_Graphics->Terminate();
}

#pragma region I/O

bool MyApp::IsKeyDown(VirtualKey k) {
	CoreVirtualKeyStates state = CoreWindow::GetForCurrentThread()->GetAsyncKeyState(k);	
	return bool(state & CoreVirtualKeyStates::Down);
}

void MyApp::ProcessInput(INPUT_MAP_ACTION key, double delta) {
	bool mod = IsKeyDown(g_InputMapAction[MODIFIER]);
	bool mod_sec = IsKeyDown(g_InputMapAction[MODIFIER_SEC]);
	delta *= Application::m_GlobalSpeedModifier * ( mod ? m_GlobalModSpeedModifier : 1);
	g_SelectedEntity->SetDirty(true);
	switch(key) {
	case FORWARD: 
		if(mod_sec)
			g_SelectedEntity->Transform(TRANSFORMATION_TYPE::ROTATE, DIRECTION::WORLD_UP, m_RotationGlobalModifier);
		else
			g_SelectedEntity->Transform(TRANSFORMATION_TYPE::TRANSLATE, DIRECTION::WORLD_FORWARD, static_cast<float>(delta));
		break;
	case BACKWARDS:
		if (mod_sec)
			g_SelectedEntity->Transform(TRANSFORMATION_TYPE::ROTATE, DIRECTION::WORLD_DOWN, m_RotationGlobalModifier);
		else
			g_SelectedEntity->Transform(TRANSFORMATION_TYPE::TRANSLATE, DIRECTION::WORLD_BACKWARDS, static_cast<float>(delta));
		break;
	case LEFT_STRAFE:
		g_SelectedEntity->Transform(TRANSFORMATION_TYPE::TRANSLATE, DIRECTION::WORLD_STRAFE_LEFT, static_cast<float>(delta));
		break;
	case RIGHT_STRAFE:
		g_SelectedEntity->Transform(TRANSFORMATION_TYPE::TRANSLATE, DIRECTION::WORLD_STRAFE_RIGHT, static_cast<float>(delta));
		break;
	case LEFT:
		g_SelectedEntity->Transform(TRANSFORMATION_TYPE::ROTATE, mod ? DIRECTION::WORLD_LEFT : DIRECTION::LOCAL_LEFT, m_RotationGlobalModifier);
		break;
	case RIGHT:
		g_SelectedEntity->Transform(TRANSFORMATION_TYPE::ROTATE, mod ? DIRECTION::WORLD_RIGHT : DIRECTION::LOCAL_RIGHT, m_RotationGlobalModifier);
		break;
	}
}

void MyApp::ProcessPointer(Point p, VirtualKey k, POINT_EVENT_TYPE t, int wheel) {
	switch(t) {
	case POINT_EVENT_TYPE::DOWN:
		if (g_InputMapPointer[CLICK_ACTION] == k) {
			// working ok, add behavior
		} else if (g_InputMapPointer[CLICK_SELECT] == k) {
			// working ok, add behavior
		}
		break;
	case POINT_EVENT_TYPE::DRAG:
		if(g_InputMapPointer[CLICK_DRAG_ACTION] == k) {
			float x = g_LastPointerData.X - p.X;
			float y = g_LastPointerData.Y - p.Y;
			// the mouse speed will be global modifier * x/yDelta
			g_SelectedEntity->Transform(TRANSFORMATION_TYPE::ROTATE, 
				(m_PointerInvert*x) > 0 ? DIRECTION::WORLD_LEFT : DIRECTION::WORLD_RIGHT, m_PointerSens*abs(x));
			g_SelectedEntity->Transform(TRANSFORMATION_TYPE::ROTATE, 
				(m_PointerInvert*y) < 0 ? DIRECTION::WORLD_DOWN : DIRECTION::WORLD_UP, m_PointerSens*abs(y));
		}
		break;
	case POINT_EVENT_TYPE::WHEEL:
		g_SelectedEntity->Transform(TRANSFORMATION_TYPE::TRANSLATE, wheel > 0 ? DIRECTION::WORLD_FORWARD : DIRECTION::WORLD_BACKWARDS);
	}
	g_LastPointerData = p;
}

#pragma endregion I/O