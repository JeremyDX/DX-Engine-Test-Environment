
#pragma once

#include "Constants.h"

using namespace DirectX;

class CameraEngine
{
	public:
		XMFLOAT2 GetDegreeVector(int degrees);
		float GetCosF(float f);
		float GetCosine(int lookup);
		float GetSin(int lookup);
		void SetProjection(XMMATRIX *matrix);
		void UpdateViewMatrix();
		void InitializeCameraPosition();

		bool ProcessCameraChanges();

	public:
		static const uint16 DEGREE_LOOKUP_TABLE[91];

	public:
		XMFLOAT4X4 view_matrix;
		XMFLOAT4X4 projection;

		XMFLOAT3 position;  //Camera Current Position on the Map.

		Short3 rotation;  //Stores the rotation values prereduced in 32bit integer form.

		XMFLOAT3 up;		//Up View Vector. This Vector tells us where to rotate the Forward/Right Vectors.

		XMFLOAT3 forward;   //Forward View Vector. Up Vector DOES NOT modify this vector.
		XMFLOAT3 right;     //Right View Vector. Up Vector DOES NOT modify this vector.
};
