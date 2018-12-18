#include "pch.h"
#include "CameraEngine.h"
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

uint16 CameraEngine::DEGREE_LOOKUP_TABLE[91] =
{
	64000, 63990, 63961, 63912, 63844, 63756,
	63649, 63523, 63377, 63212, 63028, 62824,
	62601, 62360, 62099, 61819, 61521, 61204,
	60868, 60513, 60140, 59749, 59340, 58912,
	58467, 58004, 57523, 57024, 56509, 55976,
	55426, 54859, 54275, 53675, 53058, 52426,
	51777, 51113, 50433, 49737, 49027, 48301,
	47561, 46807, 46038, 45255, 44458, 43648,
	42824, 41988, 41138, 40277, 39402, 38516,
	37618, 36709, 35788, 34857, 33915, 32962,
	32000, 31028, 30046, 29055, 28056, 27048,
	26031, 25007, 23975, 22936, 21889, 20836,
	19777, 18712, 17641, 16564, 15483, 14397,
	13306, 12212, 11113, 10012, 8907,  7800,
	6690,  5578,  4464,  3350,  2234,  1117,
	0
};

XMFLOAT2 CameraEngine::GetDegreeVector(int degree)
{
	//350.
	int quadrant = (int)(degree / 90) % 4; //3
	int degrees = degree - quadrant * 90; //80 
	float u = (float)(DEGREE_LOOKUP_TABLE[degrees] * 0.000015625);      //Value @ 80.
	float v = (float)(DEGREE_LOOKUP_TABLE[90 - degrees] * 0.000015625); //Value @ 10.
	if (quadrant == 0)
		return XMFLOAT2(u, v);
	else {
		if (quadrant & 1)
		{
			float tmp = u;
			if (quadrant == 1)
			{
				u = -v;
				v = tmp;
			}
			else {
				u = v;
				v = -tmp;
			}
			return XMFLOAT2(u, v);
		}
		else {
			return XMFLOAT2(-u, -v);
		}
	}
}

float CameraEngine::GetCosine(int lookup)
{
	int quadrant = (lookup / 90) % 4;
	int degree = (lookup - (quadrant * 90));

	if (quadrant == 0)
		return (float)(DEGREE_LOOKUP_TABLE[degree] * 0.000015625);
	else if (quadrant == 1)
		return (float)(-DEGREE_LOOKUP_TABLE[90 - degree] * 0.000015625);
	else if (quadrant == 2)
		return (float)(-DEGREE_LOOKUP_TABLE[degree] * 0.000015625);
	else if (quadrant == 3)
		return (float)(DEGREE_LOOKUP_TABLE[90 - degree] * 0.000015625);
}

float CameraEngine::GetSin(int lookup)
{
	int quadrant = (lookup / 90) % 4;
	int degree = (lookup - (quadrant * 90));

	if (quadrant == 0)
		return (float)(DEGREE_LOOKUP_TABLE[90 - degree] * 0.000015625);
	else if (quadrant == 1)
		return (float)(DEGREE_LOOKUP_TABLE[degree] * 0.000015625);
	else if (quadrant == 2)
		return (float)(-DEGREE_LOOKUP_TABLE[90 - degree] * 0.000015625);
	else if (quadrant == 3)
		return (float)(-DEGREE_LOOKUP_TABLE[degree] * 0.000015625);
}


void CameraEngine::SetProjection(XMMATRIX *p_matrix)
{
	XMStoreFloat4x4(&projection, *p_matrix);
}

void CameraEngine::UpdateViewMatrix()
{

}

//Cos(0deg)   =  1
//Cos(90deg)  =  0
//Cos(180deg) = -1
//Cos(270deg) =  0

//Sin(0deg)   =  0
//Sin(90deg)  =  1
//Sin(180deg) =  0
//Sin(270deg) = -1
//Sin(360deg) =  0

//Cos(0) = 1.0 , Cos(30) = 0.866 , Has 0.134 Traversal. Smaller Initial Movement From 1 to 0.
//Sin(0) = 0.0 , Sin(30) = 0.500 , Has 0.500 Traveraal. Larger Initial Movement From 0 to 1.

void CameraEngine::InitializeCameraPosition()
{
	int Rotation = 30;

	forward.z = GetCosine(Rotation); //-0.17
	forward.x = GetSin(Rotation);    // 0.98

	right.z = GetCosine(Rotation + 90);    // 0.98
	right.x = GetSin(Rotation + 90);       // 0.17

	up.y = GetCosine(0);

	//Z Axis aka Forward Vector.
	view_matrix._31 = forward.x;
	view_matrix._32 = 0;
	view_matrix._33 = forward.z;

	//X Axis aka Right Vector.
	view_matrix._11 = right.x;
	view_matrix._12 = 0;
	view_matrix._13 = right.z;

	//Y Axis aka Up Vector.
	view_matrix._21 = 0;
	view_matrix._22 = up.y;
	view_matrix._23 = 0;

	//W Axis aka Posisition Dot Product. 
	view_matrix._41 = position.x * view_matrix._11 + position.y * view_matrix._21 + position.z * view_matrix._31;
	view_matrix._42 = position.x * view_matrix._12 + position.y * view_matrix._22 + position.z * view_matrix._32;
	view_matrix._43 = position.x * view_matrix._13 + position.y * view_matrix._23 + position.z * view_matrix._33;

	//Not Sure Unused atm. 
	view_matrix._14 = 0.0F;
	view_matrix._24 = 0.0F;
	view_matrix._34 = 0.0F;
	view_matrix._44 = 1.0F;
}
