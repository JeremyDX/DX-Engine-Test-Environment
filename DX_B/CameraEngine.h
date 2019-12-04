#pragma once

class CameraEngine
{
	public:
		static DirectX::XMFLOAT4X4 final_result;

	public:
		static void ResetPrimaryCameraMatrix(const int FACE_DIRECTION);
		static bool PrimaryCameraUpdatedLookAt();

	private:
		static void BuildPrimaryCameraMatrix();
		static void UpdatePromptView();
};
