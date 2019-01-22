#include "pch.h"
#include "CameraEngine.h"
#include "XGameInput.h"
#include "GameTime.h"
#include "ScreenManager.h"

using namespace DirectX;

const static float HALF_DEGREE_AS_RADIAN = 0.0087266462599;

void CreateProjectionMatrix(XMFLOAT4X4 & matrix, float angle, float znear, float zfar)
{
	ZeroMemory(&matrix, sizeof(XMFLOAT4X4));

	float aspect = ScreenManager::ASPECT_RATIO;

	matrix._22 = 1.0f / tan(HALF_DEGREE_AS_RADIAN * angle);
	matrix._11 = matrix._22 / aspect;
	matrix._33 = zfar / (zfar - znear);
	matrix._34 = 1.0F;
	matrix._43 = -znear * zfar / (zfar - znear);
}

XMFLOAT4X4 CameraEngine::final_result;

//Primary Camera Sub Components.
Float3 up;
Float3 forward;
Float3 right;

//Player Position In World Space.
Float3 position;

//The Look Left/Right Rotation and Look Up/Down Rotation
Int2 rotation_data;

//Primary Camera Matrix.
XMFLOAT4X4 camera_matrix;

void BuildPrimaryCameraMatrix()
{
	//Right Vector.
	camera_matrix._11 = right._1;
	camera_matrix._21 = 0.0F;
	camera_matrix._31 = right._3;

	//Up Vector.
	camera_matrix._12 = right._2 * forward._1;
	camera_matrix._22 = up._2;
	camera_matrix._32 = right._2 * forward._3;

	//Forward Vector.
	camera_matrix._13 = forward._1 * up._2;
	camera_matrix._23 = -right._2;
	camera_matrix._33 = forward._3 * up._2;

	//Dot Product (Position, Right Vector).
	camera_matrix._41 = -(position._1 * camera_matrix._11 + position._2 * camera_matrix._21 + position._3 * camera_matrix._31);

	//Dot Product (Position, Up Vector).
	camera_matrix._42 = -(position._1 * camera_matrix._12 + position._2 * camera_matrix._22 + position._3 * camera_matrix._32);

	//Dot Product (Position, Forward Vector).
	camera_matrix._43 = -(position._1 * camera_matrix._13 + position._2 * camera_matrix._23 + position._3 * camera_matrix._33);
}

void CreateFinalMatrixResult()
{
	static const XMMATRIX PROJECTION_MATRIX = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), ScreenManager::ASPECT_RATIO, 1, 1000);
	DirectX::XMStoreFloat4x4(&CameraEngine::final_result, DirectX::XMLoadFloat4x4(&camera_matrix) * PROJECTION_MATRIX);
}

static int jump_index = 0;
static Short2 jump_strength = { 0, 0};
static Float4 jump_vector = { 0.0f, 0.0f, 0.0f, 0.0f };
static bool jump_throttle = false;
static bool sprinting = false;

void CameraEngine::Jump()
{
	int64_t temp = GameTime::AbsoluteFrameTicks();
	int jump_frame = temp - jump_index;
	if (jump_frame <= 40)
		return;
	jump_throttle = jump_frame <= 70;
	jump_index = temp;
}

void CameraEngine::ResetPrimaryCameraMatrix()
{
	position._1 = 0.0F;
	position._2 = 1.1F;
	position._3 = 0.0F;

	rotation_data._1 = 0;
	rotation_data._2 = 0;

	forward._1 = 0.0f;
	forward._2 = 0.0f;
	forward._3 = 1.0f;

	right._1 = 1.0f;
	right._2 = 0.0f;
	right._3 = 0.0f;

	up._1 = 0.0f;
	up._2 = 1.0f;
	up._3 = 0.0f;

	BuildPrimaryCameraMatrix();

	//Not Sure Unused atm. 
	camera_matrix._14 = 0.0F;
	camera_matrix._24 = 0.0F;
	camera_matrix._34 = 0.0F;
	camera_matrix._44 = 1.0F;

	CreateFinalMatrixResult();
}

bool CameraEngine::PrimaryCameraUpdatedLookAt()
{
	if (XGameInput::AnyOfTheseButtonsArePressed(XBOX_CONTROLLER::A_BUTTON))
		CameraEngine::Jump();

	int jump_frame = GameTime::AbsoluteFrameTicks() - jump_index;

	bool jumping = jump_frame <= 40;
	bool block_movement = jump_frame <= 50;

	if (XGameInput::AnyOfTheseButtonsArePressed(XBOX_CONTROLLER::LEFT_STICK_CLICK))
		sprinting = !jumping;

	XINPUT_GAMEPAD loaded_pad = XGameInput::GamePad();

	signed int right_strength = loaded_pad.sThumbLX; //Move Left Right Joystick.
	signed int forward_strength = loaded_pad.sThumbLY; //Move Forward Back Joystick.

	if (sprinting)
	{
		if (forward_strength > 24000)
			forward_strength *= 2;
		else
			sprinting = false;
	}
	if (jumping)
	{
		int jump_distance = 400 - (jump_frame - 20) * (jump_frame - 20);
		position._2 = (float)(1.1 + jump_distance * 0.0015);

		if (jump_frame == 0)
		{
			jump_strength._1 = right_strength * 0.5f;
			jump_strength._2 = forward_strength * 0.5f;
			jump_vector._1 = forward._1;
			jump_vector._2 = forward._3;
			jump_vector._3 = right._1;
			jump_vector._4 = right._3;
			sprinting = false;
		}
		else 
		{
			if (jump_throttle)
			{
				right_strength = jump_strength._1 * 0.5f;
				forward_strength = jump_strength._2 * 0.5f;
			}
			else {
				right_strength = jump_strength._1;
				forward_strength = jump_strength._2;
			}
		}
	}
	else {
		position._2 = (float) 1.1;
		jump_strength._1 = 0;
		jump_strength._2 = 0;
	}

	signed short turn_strength = loaded_pad.sThumbRX; //Turn Left Right Joystick.
	signed short look_strength = loaded_pad.sThumbRY; //Look Up Down Joystick.

	unsigned int abs_forward = forward_strength * forward_strength;
	unsigned int abs_right = right_strength * right_strength;
	unsigned int abs_turn = turn_strength * turn_strength;
	unsigned int abs_look = look_strength * look_strength;

	unsigned int dead_zone = 6400 * 6400; //Should be about at least 20% of joystick needs to be pressed. 

	bool updated = false;

	//If the player did in fact turn updated the camera movement vectors. 
	if (abs_turn > dead_zone)
	{
		rotation_data._2 += (turn_strength * (long)GameTime::DeltaTime_Micro()) / 5000.0f;
		//rotation.y = 32768 * 10;

		//0xB40000 = 360 * 32768 in hex form.
		if (rotation_data._2 < 0)
			rotation_data._2 += 0xB40000;
		if (rotation_data._2 >= 0xB40000)
			rotation_data._2 -= 0xB40000;

		float value = rotation_data._2 / 32768.0F;
		double PI = 3.14159265;

		forward._1 = (float)sin(value * PI / 180);
		forward._3 = (float)cos(value * PI / 180);

		int var = (rotation_data._2 + 0x2D0000);
		if (var >= 0xB40000)
			var -= 0xB40000;
		value = var / 32768.0F;

		right._1 = (float)sin(value * PI / 180);  //Right Vector X without Up Vector.
		right._3 = (float)cos(value * PI / 180);  //Right Vector Z without Up Vector.

		updated = true;
	}

	if (abs_look > dead_zone)
	{
		rotation_data._1 -= (look_strength * (long)GameTime::DeltaTime_Micro()) / 5000.0f;
		//rotation_data.x = 32768 * 10;

		//0x230000 = Hex version of 70.0.
		//We force the Look rotation to a range of -70 degrees and 70 degrees.
		if (rotation_data._1 < -0x230000)
			rotation_data._1 = -0x230000;
		if (rotation_data._1 > 0x230000)
			rotation_data._1 = 0x230000;

		signed int camera_rotation = rotation_data._1;
		double PI = 3.14159265;

		//Now we have a postive value between 0-70 (positive) or 290-359 (negative). 
		if (camera_rotation < 0)
			camera_rotation += 0xB40000;

		float value = camera_rotation / 32768.0F;

		right._2 = (float)sin(value * PI / 180);
		up._2 = (float)cos(value * PI / 180);

		updated = true;
	}

	if (abs_forward > dead_zone)
	{
		float strength = (float)(forward_strength * (long)GameTime::DeltaTime_Micro()) / 6400000000L;
		if (jumping)
		{
			position._1 += strength * jump_vector._1;
			position._3 += strength * jump_vector._2;
			updated = true;
		}
		else if (!block_movement) {
			position._1 += strength * forward._1;
			position._3 += strength * forward._3;
			updated = true;
		}
	}

	if (abs_right > dead_zone)
	{
		float strength = (float)(right_strength * (long)GameTime::DeltaTime_Micro()) / 6400000000L;
		if (jumping)
		{
			position._1 += strength * jump_vector._3;
			position._3 += strength * jump_vector._4;
			updated = true;
		}
		else if (!block_movement) {
			position._1 += strength * right._1;
			position._3 += strength * right._3;
			updated = true;
		}
	}

	if (block_movement && !jumping)
	{
		//41 - 50
		int jump_distance = (25 - (jump_frame - 45) * (jump_frame - 45));
		float brakes = jump_distance * 0.005;
		position._2 = 1.1f - brakes;
		updated = true;
	}

	if (updated || jumping)
	{
		BuildPrimaryCameraMatrix();
		CreateFinalMatrixResult();
	}

	return updated || jumping;
}