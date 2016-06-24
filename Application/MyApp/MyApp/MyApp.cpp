#pragma once

#include "pch.h"


using namespace Application;

MyApp::MyApp() {
	g_Graphics = ref new Gfx();
}

void MyApp::Update() {
	g_Graphics->Draw();
}

void MyApp::Resize(UINT width, UINT height) {
	g_Graphics->ClientHeight = height;
	g_Graphics->ClientWidht = width;
	g_Graphics->ResizeBuffers();
}

void MyApp::Terminate() {
	g_Graphics->Terminate();
}