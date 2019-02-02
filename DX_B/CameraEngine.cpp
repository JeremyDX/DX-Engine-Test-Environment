#include "pch.h"
#include "CameraEngine.h"
#include "XGameInput.h"
#include "GameTime.h"
#include "Animation.h"
#include "ScreenManager.h"
#include "ContentLoader.h"
#include "XModelMesh.h"

using namespace DirectX;

const static float HALF_DEGREE_AS_RADIAN = 0.0087266462599;

XMFLOAT4X4 CameraEngine::final_result;

//Primary Camera Sub Components.
Float2 up;
Float2 forward;
Float2 right;
Byte2 quadrants;

//Player Position In World Space.
Float3 position;

//The Look Left/Right Rotation and Look Up/Down Rotation
Int2 rotation_data;

//Primary Camera Matrix.
XMFLOAT4X4 camera_matrix;

Float2 blocking_value[4];

int forced_animation_index = 0;
Float4 anim_move_vectors = { 0.0f, 0.0f, 0.0f, 0.0f };
Float3 base_animation_position = { 0.0f, 0.0f, 0.0f };
Int2 animation_move_strength = { 0, 0 };
bool throttled_jump = false;
bool sprinting = false;

int HORIZONTAL_SPEED = 1;
int VERTICAL_SPEED = 2;

void CreateFinalMatrixResult()
{
	static const XMMATRIX PROJECTION_MATRIX = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), ScreenManager::ASPECT_RATIO, 0.01F, 300.0F);
	DirectX::XMStoreFloat4x4(&CameraEngine::final_result, DirectX::XMLoadFloat4x4(&camera_matrix) * PROJECTION_MATRIX);
}

void CameraEngine::ResetPrimaryCameraMatrix()
{
	position._1 = 0.5F;
	position._2 = 0.0F;
	position._3 = 0.5F;

	rotation_data._1 = 0;
	rotation_data._2 = 0;

	right._1 = 1.0f;
	right._2 = 0.0f;

	up._1 = 0.0f;
	up._2 = 1.0f;

	forward._1 = 0.0f;
	forward._2 = 1.0f;

	quadrants._1 = 0;
	quadrants._2 = 0;

	BuildPrimaryCameraMatrix();

	//Not Sure Unused atm. 
	camera_matrix._14 = 0.0F;
	camera_matrix._24 = 0.0F;
	camera_matrix._34 = 0.0F;
	camera_matrix._44 = 1.0F;

	CreateFinalMatrixResult();
}

void CameraEngine::BuildPrimaryCameraMatrix()
{
	//Right Vector.
	camera_matrix._11 = right._1; //Sin(Turn)
	camera_matrix._21 = 0.0F;     //Always Flat at 0.0F.
	camera_matrix._31 = right._2; //Cos(Turn)

	//Up Vector.
	camera_matrix._12 = up._1 * forward._1;   //Sin(Look) * Sin(Turn)
	camera_matrix._22 = up._2;                //Cos(Look)
	camera_matrix._32 = up._1 * forward._2;   //Sin(Look) * Cos(Turn)

	//Forward Vector.
	camera_matrix._13 =  up._2 * forward._1; //Cos(Look) * Sin(Turn)
	camera_matrix._23 = -up._1;              //Inverse of Sin(Look)
	camera_matrix._33 =  up._2 * forward._2; //Cos(Look) * Cos(Turn)


	float xoffset = (position._1);
	float zoffset = (position._3);

	float height = (position._2 + 1.8F);

	//Dot Product (Position, Right Vector).
	camera_matrix._41 = -(xoffset * camera_matrix._11 + height * camera_matrix._21 + zoffset * camera_matrix._31);

	//Dot Product (Position, Up Vector).
	camera_matrix._42 = -(xoffset* camera_matrix._12 + height * camera_matrix._22 + zoffset * camera_matrix._32);

	//Dot Product (Position, Forward Vector).
	camera_matrix._43 = -(xoffset * camera_matrix._13 + height * camera_matrix._23 + zoffset * camera_matrix._33);
}

bool CameraEngine::PrimaryCameraUpdatedLookAt()
{
	//This is to ensure we only perform 1 frame of data, in event we are running too quick it won't do anything.
	//But if we stall then we stop updating and we don't "skip" frames to catch up. it's SOL if you lag.
	bool delta_frame = GameTime::DeltaTime_Frames() > 0;

	int64_t current_frame = GameTime::AbsoluteFrameTicks();
	Animation &ref = Animation::GetAnimation(2);

	bool updated = false;

	XINPUT_GAMEPAD& loaded_pad = XGameInput::GamePad();

	signed int right_strength = loaded_pad.sThumbLX; //Move Left Right Joystick.
	signed int forward_strength = loaded_pad.sThumbLY; //Move Forward Back Joystick.

	signed short turn_strength = loaded_pad.sThumbRX; //Turn Left Right Joystick.
	signed short look_strength = loaded_pad.sThumbRY; //Look Up Down Joystick.

	unsigned int abs_turn = turn_strength * turn_strength;
	unsigned int abs_look = look_strength * look_strength;

	unsigned int abs_forward = forward_strength * forward_strength;
	unsigned int abs_right = right_strength * right_strength;
	unsigned int dead_zone = 6400 * 6400; //Should be about at least 20% of joystick needs to be pressed. 

	if (XGameInput::AnyOfTheseButtonsArePressed(XBOX_CONTROLLER::LEFT_STICK_CLICK))
	{
		sprinting = !sprinting; //Reverse Sprint State.
	}

	if (sprinting)
	{
		sprinting = sprinting = forward_strength > 26000;
		if (sprinting)
		{
			forward_strength *= 2;
		}
	}

	if (XGameInput::AnyOfTheseButtonsArePressed(XBOX_CONTROLLER::A_BUTTON))
	{
		int change = (current_frame - forced_animation_index);
		if (change > 49)
		{
			throttled_jump = change < ref.Length + 6;
			forced_animation_index = (throttled_jump || sprinting) ? current_frame - 20 : current_frame;
			base_animation_position._1 = position._1;
			base_animation_position._2 = 0.0f;
			base_animation_position._3 = position._3;
			animation_move_strength._1 = abs_right > dead_zone ? right_strength : 0;
			animation_move_strength._2 = abs_forward > dead_zone ? forward_strength : 0;
			if (throttled_jump && !sprinting)
			{
				animation_move_strength._1 /= 2;
				animation_move_strength._2 /= 2;
			}
			anim_move_vectors._1 = right._1;
			anim_move_vectors._2 = right._2;
			anim_move_vectors._3 = forward._1;
			anim_move_vectors._4 = forward._2;
			sprinting = false;
		}
	}

	int jump_frame = current_frame - forced_animation_index;
	bool isJumping = jump_frame < ref.Length;
	bool isBlocking = jump_frame < 50;

	if (isJumping)
	{
		updated = true;
		 
		if (jump_frame <= 9)
		{
			//0,3,6,9
			position._2 = base_animation_position._2 - (0.0160 * (jump_frame + 1));
			forced_animation_index -= 2;
		}
		else if (jump_frame <= 19)
		{
			//12,15,18,
			int value = 10 - (jump_frame - 9);
			position._2 = base_animation_position._2 - (0.0160 * value);
			forced_animation_index -= 2;
		}
		else if (jump_frame <= 49)
		{
			//21 to 49 = 29 Frames.
			int value = 196 - ((jump_frame - 34) * (jump_frame - 34));
			if (throttled_jump)
				value /= 2;
			position._2 = base_animation_position._2 + ((float)value / 196);
		} 
		else if (jump_frame <= 59)
		{
			//10 Frames.
			int value = 10 - (jump_frame - 49); 
			position._2 = base_animation_position._2 - (0.0160 * value);
		}
		if (jump_frame >= 20 && jump_frame <= 49)
		{
			//1 to 30.
			float r_strength = (float)(animation_move_strength._1 * (jump_frame - 19)) / 600000L;
			float f_strength = (float)(animation_move_strength._2 * (jump_frame - 19)) / 600000L;

			position._1 = base_animation_position._1 + r_strength * anim_move_vectors._1;
			position._3 = base_animation_position._3 + r_strength * anim_move_vectors._2;

			position._1 = position._1 + f_strength * anim_move_vectors._3;
			position._3 = position._3 + f_strength * anim_move_vectors._4;
		}
	}

	//If the player did in fact turn update the camera movement vectors. 
	if (abs_turn > dead_zone)
	{
		rotation_data._2 += (turn_strength * delta_frame * HORIZONTAL_SPEED);

		if (rotation_data._2 < 0)
			rotation_data._2 += 0xAFC800;
		if (rotation_data._2 >= 0xAFC800)
			rotation_data._2 -= 0xAFC800;

		int value = rotation_data._2 / 320;
		double PI = 3.14159265;

		forward._1 = (float)sin((value * PI) / 18000);
		forward._2 = (float)cos((value * PI) / 18000);

		right._1 = (float)sin(((value + 9000) * PI) / 18000);
		right._2 = (float)cos(((value + 9000) * PI) / 18000);

		updated = true;
	}

	if (abs_look > dead_zone)
	{
		const int RANGE = 65 * 32000;

		rotation_data._1 -= (look_strength * delta_frame * VERTICAL_SPEED);

		if (rotation_data._1 < -RANGE)
			rotation_data._1 = -RANGE;
		if (rotation_data._1 > RANGE)
			rotation_data._1 = RANGE;
		
		double PI = 3.14159265;

		int value = rotation_data._1 / 320;

		up._1 = (float)sin((value * PI) / 18000);
		up._2 = (float)cos((value * PI) / 18000);

		updated = true;
	}

	if (!isBlocking)
	{
		Float2 verify = { 0.0f, 0.0f };

		if (abs_forward > dead_zone)
		{
			float strength = (float)(forward_strength * delta_frame) / 600000L;
			verify._1 += strength * forward._1; //SinX
			verify._2 += strength * forward._2; //CosZ
		}

		if (abs_right > dead_zone)
		{
			float strength = (float)(right_strength * delta_frame) / 600000L;
			verify._1 += strength * right._1; //SinX
			verify._2 += strength * right._2; //CosZ
		}

		position._1 += verify._1;
		position._3 += verify._2;

		updated = true;

		//XModelMesh::GetCurrentCollidorIndex(position);
	}

	if (position._1 < 0.5f)
		position._1 = 0.5f;
	else if (position._1 > 95.5f)
		position._1 = 95.5f;
	if (position._3 < 0.5f)
		position._3 = 0.5f;
	else if (position._3 > 95.5f)
		position._3 = 95.5f;

	if (updated)
	{
		BuildPrimaryCameraMatrix();
		CreateFinalMatrixResult();
	}

	return updated;
}

void CameraEngine::CreateDebugOverlay()
{
	char buffer[12];

	snprintf(buffer, 12, "%f", position._1);
	ContentLoader::UpdateOverlayString(13 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", position._2);
	ContentLoader::UpdateOverlayString(38 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", position._3);
	ContentLoader::UpdateOverlayString(63 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", blocking_value[2]._1);
	ContentLoader::UpdateOverlayString(89 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", blocking_value[2]._2);
	ContentLoader::UpdateOverlayString(115 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", 0.0F);
	ContentLoader::UpdateOverlayString(138 * 6, buffer, 12 * 6);
}

/*
* This is unused but shows the process to creating the projection matrix and setting matrix data accordingly.
*/
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