
#pragma once

#include "Constants.h"

class CameraEngine
{
	public:
		static void ResetPrimaryCameraMatrix(const int FACE_DIRECTION);
		static bool PrimaryCameraUpdatedLookAt();
		static void CreateDebugOverlay();

	private:
		static void BuildPrimaryCameraMatrix();

	public:
		static DirectX::XMFLOAT4X4 final_result;
};
