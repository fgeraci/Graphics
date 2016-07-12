#pragma once

#include "pch.h"

using namespace DirectX;

namespace Application {
	
	// Application	- MyApp
	const static UINT		m_MinWidth  = 800;
	const static UINT		m_MinHeight = 600;
	const static int		m_GlobalFPS = 60;
	const static float		m_GlobalSpeedModifier = 0.05f;				// Global speed adjuster
	const static float		m_GlobalModSpeedModifier = 2.0f;			// Speed on shift
	const static float		m_RotationGlobalModifier = 0.15f;			// Rotation adjuster
	const static float		m_FloatingPointErrorTolerance = 0.001f;		

	// Input
	const static int		m_LeftScreenSide	=  1;
	const static int		m_RightScreenSide	= -1;
	const static int		m_UpScreenSide		= -1;
	const static int		m_DownScreenSide	=  1;
	const static int		m_PointerInvert		=  1;		// 1 --> false
	const static float		m_PointerSens		=  5.0f;

	// Graphics		- Gfx
	const static XMVECTOR	m_WorldUpVector		 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const static XMVECTOR	m_WorldRightVector	 = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	const static XMVECTOR	m_WorldForwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
}