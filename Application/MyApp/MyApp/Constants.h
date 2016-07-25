#pragma once

#include "pch.h"

using namespace DirectX;

namespace Application {
	
	// Application	- MyApp
	const static UINT		m_MinWidth  = 800;
	const static UINT		m_MinHeight = 600;
	const static int		m_GlobalFPS = 60;
	const static float		m_GlobalSpeedModifier = 1.0f;				// Global speed adjuster
	const static float		m_GlobalModSpeedModifier = 1.5f;			// Speed on shift
	const static float		m_RotationGlobalModifier = 1.0f;			// Rotation adjuster in degrees
	const static float		m_FloatingPointErrorTolerance = 0.001f;		

	// Input
	const static int		m_LeftScreenSide	=  1;
	const static int		m_RightScreenSide	= -1;
	const static int		m_UpScreenSide		= -1;
	const static int		m_DownScreenSide	=  1;
	const static int		m_PointerInvert		=  1;		// 1 --> false
	const static float		m_PointerSens		=  0.06f;
	const static float		m_ViewAngleVertical =  0.25f;
	const static float		m_DepthOfViewNearZ	=  0.25f;
	const static float		m_DepthOfViewFarZ	=  1000.0f;

	// Graphics		- Gfx
	const static XMVECTOR	m_WorldUpVector		 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const static XMVECTOR	m_WorldRightVector	 = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	const static XMVECTOR	m_WorldForwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
}