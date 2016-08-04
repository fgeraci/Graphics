#pragma once

#include "pch.h"


using namespace Application;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Application::Enums::Transformation;
using namespace Application::Enums::Input;

MyApp::MyApp() {
	LOGMESSAGE(L"Initializing MyApp ...\n");
	g_Paused = false;
	g_Graphics = new Gfx();
	InitializeInput();
	g_SelectedEntity = g_Graphics->MainCamera();
	
	g_Entities.push_back(g_Graphics->AddPolygon(POLYGON_TYPE::GRID, false));

	// add test polygon
	//g_Entities.push_back(g_Graphics->AddPolygon(POLYGON_TYPE::CUBE, true));
	//g_Entities[0]->SetParent(g_Graphics->WorldEntityParent());
	//g_Entities[0]->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::UP, TRANSFORM_HIERARCHY::WORLD);
	//g_Entities[0]->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::RIGHT, TRANSFORM_HIERARCHY::LOCAL, 10.0f);
	//g_Entities.push_back(g_Graphics->AddPolygon(POLYGON_TYPE::CUBE, true));
	//
	//g_Entities[1]->SetParent(g_Graphics->WorldEntityParent());
	//g_Entities[1]->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::LEFT, TRANSFORM_HIERARCHY::LOCAL, 10.0f);
	//g_Entities[1]->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::UP, TRANSFORM_HIERARCHY::WORLD);
	//
	//g_Entities.push_back(g_Graphics->AddPolygon(POLYGON_TYPE::CUBE, true));
	//g_Entities[2]->SetParent(g_Graphics->WorldEntityParent());
	//g_Entities[2]->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::FORWARD, TRANSFORM_HIERARCHY::LOCAL, 10.0f);
	//g_Entities[2]->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::UP, TRANSFORM_HIERARCHY::WORLD);
	//
	//g_Entities.push_back(g_Graphics->AddPolygon(POLYGON_TYPE::CUBE, true));
	//g_Entities[3]->SetParent(g_Graphics->WorldEntityParent());
	//g_Entities[3]->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::BACKWARDS, TRANSFORM_HIERARCHY::LOCAL, 10.0f);
	//g_Entities[3]->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::UP, TRANSFORM_HIERARCHY::WORLD);
	//
	//g_Entities.push_back(g_Graphics->AddPolygon(POLYGON_TYPE::CYLINDER, true));
	//g_Entities[4]->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::UP, TRANSFORM_HIERARCHY::WORLD, 5.0f);
	//
	g_Entities.push_back(g_Graphics->AddPolygon(POLYGON_TYPE::ICOSAHEDRON, true));
	// add grid
	
}

void MyApp::Update(Ticker* t) {
	
	// process keyboard input
	if(t->Tick()) {
		ReadAsyncKeys();

		// handle entities
		UpdateLogic();

		// render
		g_Graphics->Draw();
	}

}

void MyApp::UpdateLogic() {
	for(int i = 0; i < g_Entities.size(); ++i) {
		/* Test */
		if (g_Entities[i]->Name() == L"Cube") {
			g_Entities[i]->Transform(TRANSFORMATION_TYPE::ROTATE, TRANSFORM_DIRECTION::LEFT, TRANSFORM_HIERARCHY::WORLD);
			g_Entities[i]->Transform(TRANSFORMATION_TYPE::ROTATE, TRANSFORM_DIRECTION::RIGHT, TRANSFORM_HIERARCHY::LOCAL, 5.0f);
		}
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
	g_InputMapUI[INPUT_MAP_UI::OPTIONS] = VirtualKey::O;
	g_InputMapUI[INPUT_MAP_UI::ESCAPE] = VirtualKey::Escape;
	g_InputMapUI[INPUT_MAP_UI::PAUSE] = VirtualKey::P;
	// Action map
	g_InputMapAction[INPUT_MAP_ACTION::FORWARD] = VirtualKey::W;
	g_InputMapAction[INPUT_MAP_ACTION::BACKWARDS] = VirtualKey::S;
	g_InputMapAction[INPUT_MAP_ACTION::LEFT] = VirtualKey::Q;
	g_InputMapAction[INPUT_MAP_ACTION::RIGHT] = VirtualKey::E;
	g_InputMapAction[INPUT_MAP_ACTION::LEFT_STRAFE] = VirtualKey::A;
	g_InputMapAction[INPUT_MAP_ACTION::RIGHT_STRAFE] = VirtualKey::D;
	g_InputMapAction[INPUT_MAP_ACTION::MODIFIER] = VirtualKey::LeftShift;
	g_InputMapAction[INPUT_MAP_ACTION::MODIFIER_SEC] = VirtualKey::LeftControl;
	// Pointer map
	g_InputMapPointer[INPUT_MAP_POINTER::CLICK_ACTION] = VirtualKey::RightButton;
	g_InputMapPointer[INPUT_MAP_POINTER::CLICK_SELECT] = VirtualKey::LeftButton;
	g_InputMapPointer[INPUT_MAP_POINTER::CLICK_DRAG_ACTION] = VirtualKey::RightButton;
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
	case INPUT_MAP_ACTION::FORWARD:
		if(mod_sec)
			g_SelectedEntity->Transform(TRANSFORMATION_TYPE::ROTATE, TRANSFORM_DIRECTION::UP, TRANSFORM_HIERARCHY::LOCAL, m_RotationGlobalModifier);
		else
			g_SelectedEntity->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::FORWARD, TRANSFORM_HIERARCHY::WORLD,static_cast<float>(delta));
		break;
	case INPUT_MAP_ACTION::BACKWARDS:
		if (mod_sec)
			g_SelectedEntity->Transform(TRANSFORMATION_TYPE::ROTATE, TRANSFORM_DIRECTION::DOWN, TRANSFORM_HIERARCHY::LOCAL, m_RotationGlobalModifier);
		else
			g_SelectedEntity->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::BACKWARDS, TRANSFORM_HIERARCHY::WORLD, static_cast<float>(delta));
		break;
	case INPUT_MAP_ACTION::LEFT_STRAFE:
		g_SelectedEntity->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::STRAFE_LEFT, TRANSFORM_HIERARCHY::WORLD, static_cast<float>(delta));
		break;
	case INPUT_MAP_ACTION::RIGHT_STRAFE:
		g_SelectedEntity->Transform(TRANSFORMATION_TYPE::TRANSLATE, TRANSFORM_DIRECTION::STRAFE_RIGHT, TRANSFORM_HIERARCHY::WORLD, static_cast<float>(delta));
		break;
	case INPUT_MAP_ACTION::LEFT:
		g_SelectedEntity->Transform(TRANSFORMATION_TYPE::ROTATE, TRANSFORM_DIRECTION::LEFT, TRANSFORM_HIERARCHY::WORLD, m_RotationGlobalModifier);
		break;
	case INPUT_MAP_ACTION::RIGHT:
		g_SelectedEntity->Transform(TRANSFORMATION_TYPE::ROTATE, TRANSFORM_DIRECTION::RIGHT, TRANSFORM_HIERARCHY::WORLD, m_RotationGlobalModifier);
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
				(m_PointerInvert*x) > 0 ? TRANSFORM_DIRECTION::LEFT : TRANSFORM_DIRECTION::RIGHT, TRANSFORM_HIERARCHY::WORLD, m_PointerSens*abs(x));
			g_SelectedEntity->Transform(TRANSFORMATION_TYPE::ROTATE, 
				(m_PointerInvert*y) < 0 ? TRANSFORM_DIRECTION::DOWN : TRANSFORM_DIRECTION::UP, TRANSFORM_HIERARCHY::LOCAL,m_PointerSens*abs(y));
		}
		break;
	case POINT_EVENT_TYPE::WHEEL:
		g_SelectedEntity->Transform(TRANSFORMATION_TYPE::TRANSLATE, wheel > 0 ? TRANSFORM_DIRECTION::FORWARD : TRANSFORM_DIRECTION::BACKWARDS,
			TRANSFORM_HIERARCHY::WORLD,1.0f);
	}
	g_LastPointerData = p;
}

#pragma endregion I/O