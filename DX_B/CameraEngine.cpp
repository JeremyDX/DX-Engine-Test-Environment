#include "pch.h"
#include "CameraEngine.h"
#include "XGameInput.h"

#pragma comment (lib, "xinput.lib")

const uint16 CameraEngine::DEGREE_LOOKUP_TABLE[91] =
{
	64000, 63990, 63961, 63912, 63844, 63756, //0 - 5
	63649, 63523, 63377, 63212, 63028, 62824, //6 - 11
	62601, 62360, 62099, 61819, 61521, 61204, //12 - 17
	60868, 60513, 60140, 59749, 59340, 58912, //18 - 23
	58467, 58004, 57523, 57024, 56509, 55976, //24 - 29
	55426, 54859, 54275, 53675, 53058, 52426, //30 - 35
	51777, 51113, 50433, 49737, 49027, 48301, //36 - 41
	47561, 46807, 46038, 45255, 44458, 43648, //42 - 47
	42824, 41988, 41138, 40277, 39402, 38516, //48 - 53
	37618, 36709, 35788, 34857, 33915, 32962, //54 - 59
	32000, 31028, 30046, 29055, 28056, 27048, //60 - 65
	26031, 25007, 23975, 22936, 21889, 20836, //66 - 71
	19777, 18712, 17641, 16564, 15483, 14397, //72 - 77
	13306, 12212, 11113, 10012, 8907,  7800,  //78 - 83
	6690,  5578,  4464,  3350,  2234,  1117,  //84 - 89
	0										  //90
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


//Returns a more concise cosine variable position.
float CameraEngine::GetCosF(float f)
{
	int quadrant = ((int)(f / 90)) % 4;
	int degree = (int)(f);
	float remainder = f - degree;
	degree -= (quadrant * 90);

	//Quadrant = 0-3, I = 0-89 Main Degree, R = 0-89 Decimal Degree Index.
	if (quadrant == 0)
	{
		return (float)(DEGREE_LOOKUP_TABLE[degree] * 0.000015625) + 
			((DEGREE_LOOKUP_TABLE[degree] - DEGREE_LOOKUP_TABLE[degree + 1]) * remainder);
	}
	else if (quadrant == 1)
	{
		return (float)(-DEGREE_LOOKUP_TABLE[90 - degree] * 0.000015625) - 
			((DEGREE_LOOKUP_TABLE[90 - degree] - DEGREE_LOOKUP_TABLE[90 - degree + 1]) * remainder);
	}
	else if (quadrant == 2)
	{
		return (float)(-DEGREE_LOOKUP_TABLE[degree] * 0.000015625) - 
			((DEGREE_LOOKUP_TABLE[degree] - DEGREE_LOOKUP_TABLE[degree + 1]) * remainder);
	}
	else if (quadrant == 3)
	{
		return (float)(DEGREE_LOOKUP_TABLE[90 - degree] * 0.000015625) + 
			((DEGREE_LOOKUP_TABLE[90 - degree] - DEGREE_LOOKUP_TABLE[90 - degree + 1]) * remainder);
	}
}

//Operation does not support negative input no checks. 
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
	return 0.0F;
}

//Operation does not support negative input no checks. 
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
	return 0.0F;
}


void CameraEngine::SetProjection(XMMATRIX *p_matrix)
{
	XMStoreFloat4x4(&projection, *p_matrix);
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
	position.x = 0.0F;
	position.y = 0.0F;
	position.z = 0.0F;

	rotation._1 = 0;
	rotation._2 = 0;
	rotation._3 = 0;

	forward.x = 0.0f;
	forward.y = 0.0f;
	forward.z = 1.0f;

	right.x = 1.0f;
	right.y = 0.0f;
	right.z = 0.0f;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//Z Axis aka Forward Vector.
	view_matrix._31 = 0;
	view_matrix._32 = 0;
	view_matrix._33 = 1;

	//X Axis aka Right Vector.
	view_matrix._11 = 1;
	view_matrix._12 = 0;
	view_matrix._13 = 0;

	//Y Axis aka Up Vector.
	view_matrix._21 = 0;
	view_matrix._22 = 1;
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

	OutputDebugString(L"Hello World!!!!");
}

bool CameraEngine::ProcessCameraChanges()
{
	XINPUT_GAMEPAD loaded_pad = XGameInput::GamePad();

	signed short right_strength = loaded_pad.sThumbLX; //Move Left Right Joystick.
	signed short forward_strength = loaded_pad.sThumbLY; //Move Forward Back Joystick.

	signed short turn_strength = loaded_pad.sThumbRX; //Turn Left Right Joystick.
	signed short look_strength = loaded_pad.sThumbRY; //Look Up Down Joystick.

	unsigned int abs_forward = forward_strength * forward_strength;
	unsigned int abs_right = right_strength * right_strength;
	unsigned int abs_turn = turn_strength * turn_strength;
	unsigned int abs_look = look_strength * look_strength;

	unsigned int dead_zone =  6400 * 6400; //Should be about at least 20% of joystick needs to be pressed. 

	bool updated = false;

	//If the player did in fact turn updated the camera movement vectors. 
	if (abs_turn > dead_zone)
	{
		rotation._2 += turn_strength * 2;
		//rotation.y = 32768 * 10;

		//0xB40000 = 360 * 32768 in hex form.
		if (rotation._2 < 0) 
			rotation._2 += 0xB40000;
		if (rotation._2 >= 0xB40000)
			rotation._2 -= 0xB40000;

		float value = rotation._2 / 32768.0F;
		double PI = 3.14159265;

		forward.x = (float)sin(value * PI / 180);
		forward.z = (float)cos(value * PI / 180);

		int var = (rotation._2 + 0x2D0000);
		if (var >= 0xB40000)
			var -= 0xB40000;
		value = var / 32768.0F;

		right.x = (float)sin(value * PI / 180);  //Right Vector X without Up Vector.
		right.z = (float)cos(value * PI / 180);  //Right Vector Z without Up Vector.

		updated = true;
	}

	if (abs_look > dead_zone)
	{
		rotation._1 -= look_strength * 2;
		//rotation.x = 32768 * 10;

		//0x230000 = Hex version of 70.0.
		//We force the Look rotation to a range of -70 degrees and 70 degrees.
		if (rotation._1 < -0x230000)
			rotation._1 = -0x230000;
		if (rotation._1 > 0x230000)
			rotation._1 = 0x230000;

		signed int camera_rotation = rotation._1;
		double PI = 3.14159265;

		//Now we have a postive value between 0-70 (positive) or 290-359 (negative). 
		if (camera_rotation < 0)
			camera_rotation += 0xB40000;

		float value = camera_rotation / 32768.0F;

		right.y = (float)sin(value * PI / 180);
		up.y = (float)cos(value * PI / 180);

		updated = true;
	}

	if (abs_forward > dead_zone)
	{
		float strength = (float)(forward_strength * 0.00003125 * 0.15);
		position.x += strength * forward.x;
		position.z += strength * forward.z;
		updated = true;
	}

	if (abs_right > dead_zone)
	{
		float strength = (float)(right_strength * 0.00003125 * 0.15);
		position.x += strength * right.x;
		position.z += strength * right.z;
		updated = true;
	}

	return updated;
}


//right.y IS NOT in use rather it's a placeholder for Y sin vlaue.
void CameraEngine::UpdateViewMatrix()
{
	//Right Vector.
	view_matrix._11 = right.x;
	view_matrix._21 = 0.0F;
	view_matrix._31 = right.z;

	//Up Vector.
	view_matrix._12 = right.y * forward.x;
	view_matrix._22 = up.y;
	view_matrix._32 = right.y * forward.z;

	//Forward Vector.
	view_matrix._13 = forward.x * up.y;
	view_matrix._23 = -right.y;
	view_matrix._33 = forward.z * up.y;

	//Dot Product (Position, Right Vector).
	view_matrix._41 = -(position.x * view_matrix._11 + position.y * view_matrix._21 + position.z * view_matrix._31);

	//Dot Product (Position, Up Vector).
	view_matrix._42 = -(position.x * view_matrix._12 + position.y * view_matrix._22 + position.z * view_matrix._32);

	//Dot Product (Position, Forward Vector).
	view_matrix._43 = -(position.x * view_matrix._13 + position.y * view_matrix._23 + position.z * view_matrix._33);
}
