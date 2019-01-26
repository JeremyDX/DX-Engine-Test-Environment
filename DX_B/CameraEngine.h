
#pragma once

#include "Constants.h"

class CameraEngine
{
	public:
		static void ResetPrimaryCameraMatrix();
		static bool PrimaryCameraUpdatedLookAt();
		static void CreateDebugOverlay();

	public:
		static DirectX::XMFLOAT4X4 final_result;
};
