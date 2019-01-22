
#pragma once

#include "Constants.h"

class CameraEngine
{
	public:
		static void Jump();
		static void ResetPrimaryCameraMatrix();
		static bool PrimaryCameraUpdatedLookAt();

	public:
		static DirectX::XMFLOAT4X4 final_result;
};
