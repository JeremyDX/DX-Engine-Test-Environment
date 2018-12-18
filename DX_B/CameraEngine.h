#pragma once

using namespace DirectX;

class CameraEngine
{
	public:
		XMFLOAT2 GetDegreeVector(int degrees);
		float GetCosine(int lookup);
		float GetSin(int lookup);
		void SetProjection(XMMATRIX *matrix);
		void UpdateViewMatrix();
		void InitializeCameraPosition();

	private:
		static uint16 DEGREE_LOOKUP_TABLE[91];

	public:
		XMFLOAT4X4 view_matrix;
		XMFLOAT4X4 projection;

		XMFLOAT3 position;
		XMFLOAT3 up;
		XMFLOAT3 forward;
		XMFLOAT3 right;
};