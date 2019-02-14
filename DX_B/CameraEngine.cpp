#include "pch.h"
#include "CameraEngine.h"
#include "XGameInput.h"
#include "GameTime.h"
#include "Animation.h"
#include "ScreenManager.h"
#include "ContentLoader.h"
#include "XModelMesh.h"

const static uint32_t DEAD_ZONE = 4800 * 4800;
const static double HALF_DEGREE_AS_RADIANS = 0.0087266462599;
const static double ONE_DEGREE_AS_RADIANS = 0.01745329251;
const static double NINETY_DEGREES_AS_RADIANS = 1.57079632679;

DirectX::XMFLOAT4X4 CameraEngine::final_result;
DirectX::XMFLOAT4X4 camera_matrix;

//Primary Camera Sub Components.
Float2 up;
Float2 forward;
Float2 right;
Float2 test;
Byte2 quadrants;

//Player Position In World Space.
Float3 player_position;
//Float3 camera_position;

//The Look Left/Right Rotation and Look Up/Down Rotation
Int4 rotation_data;

Float2 blocking_value[4];

int forced_animation_index = 0;
Float4 anim_move_vectors = { 0.0f, 0.0f, 0.0f, 0.0f };
Float3 base_animation_position = { 0.0f, 0.0f, 0.0f };
Int2 animation_move_strength = { 0, 0 };
bool throttled_jump = false;
bool sprinting = false;

bool send_camera_as_player = true;

int HORIZONTAL_SPEED = 3;
int VERTICAL_SPEED = 2;

void CreateFinalMatrixResult()
{
	static const DirectX::XMMATRIX PROJECTION_MATRIX = DirectX::XMMatrixPerspectiveFovLH(45 * ONE_DEGREE_AS_RADIANS, ScreenManager::ASPECT_RATIO, 0.1F, 300.0F);
	DirectX::XMStoreFloat4x4(&CameraEngine::final_result, DirectX::XMLoadFloat4x4(&camera_matrix) * PROJECTION_MATRIX);
}

void CameraEngine::ResetPrimaryCameraMatrix(const int FACE_DIRECTION)
{
	player_position._1 = 0.5F;
	player_position._2 = 0.0F;
	player_position._3 = 0.5F;

	rotation_data._1 = 0;
	rotation_data._2 = rotation_data._3 = FACE_DIRECTION * 32000;
	rotation_data._4 = 0;

	up._1 = 0.0f;
	up._2 = 1.0f;

	double rotation = FACE_DIRECTION * ONE_DEGREE_AS_RADIANS;

	forward._1 = (float)sin(rotation);
	forward._2 = (float)cos(rotation);

	rotation += NINETY_DEGREES_AS_RADIANS;

	right._1 = (float)sin(rotation);
	right._2 = (float)cos(rotation);

	float SinH = forward._1 * 0.125F;
	float CosH = forward._2 * 0.125F;
	float SinW = forward._1 * 0.250F;
	float CosW = forward._2 * 0.250F;

	blocking_value[0]._1 = (CosW + SinH);
	blocking_value[0]._2 = (-SinW + CosH);
	blocking_value[1]._1 = (-CosW + SinH);
	blocking_value[1]._2 = (SinW + CosH);

	blocking_value[2]._1 = -blocking_value[0]._1;
	blocking_value[2]._2 = -blocking_value[0]._2;
	blocking_value[3]._1 = -blocking_value[1]._1;
	blocking_value[3]._2 = -blocking_value[1]._2;

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

	float xoffset = (player_position._1);
	float zoffset = (player_position._3);
	float height =  (player_position._2 + 1.8);

	//Dot Product (Position, Right Vector).
	camera_matrix._41 = -(xoffset * camera_matrix._11 + height * camera_matrix._21 + zoffset * camera_matrix._31);

	//Dot Product (Position, Up Vector).
	camera_matrix._42 = -(xoffset * camera_matrix._12 + height * camera_matrix._22 + zoffset * camera_matrix._32);

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
		if (change > 29)
		{
			forced_animation_index = current_frame;
			base_animation_position._1 = player_position._1;
			base_animation_position._2 = 0.0f;
			base_animation_position._3 = player_position._3;
			animation_move_strength._1 = abs_right > DEAD_ZONE ? right_strength : 0;
			animation_move_strength._2 = abs_forward > DEAD_ZONE ? forward_strength : 0;
			anim_move_vectors._1 = right._1;
			anim_move_vectors._2 = right._2;
			anim_move_vectors._3 = forward._1;
			anim_move_vectors._4 = forward._2;
			sprinting = false;
		}
	}

	int jump_frame = current_frame - forced_animation_index;
	bool isJumping = jump_frame < ref.Length;
	bool isBlocking = jump_frame < 30;

	if (isJumping)
	{
		updated = true;
		 
		if (jump_frame <= 29)
		{
			//Frames 0 - 28.
			int value = 196 - ((jump_frame - 14) * (jump_frame - 14));
			if (!sprinting)
				value /= 2.0f;
			player_position._2 = base_animation_position._2 + ((float)value / 196);

			float r_strength = (float)(animation_move_strength._1 * (jump_frame + 1)) / 600000L;
			float f_strength = (float)(animation_move_strength._2 * (jump_frame + 1)) / 600000L;

			player_position._1 = base_animation_position._1 + r_strength * anim_move_vectors._1;
			player_position._3 = base_animation_position._3 + r_strength * anim_move_vectors._2;

			player_position._1 = player_position._1 + f_strength * anim_move_vectors._3;
			player_position._3 = player_position._3 + f_strength * anim_move_vectors._4;
		} 
		else if (jump_frame <= 39)
		{
			//10 Frames.
			int value = 10 - (jump_frame - 29); 
			player_position._2 = base_animation_position._2 - (0.0160 * value);
		}
	}

	//If the player did in fact turn update the camera movement vectors. 
	if (abs_turn > DEAD_ZONE)
	{
		int incremented = turn_strength * delta_frame * HORIZONTAL_SPEED;
		rotation_data._2 += incremented;

		int angle = rotation_data._2 - rotation_data._3 - rotation_data._4;

		if (rotation_data._2 < 0)
		{
			rotation_data._2 += 11520000;
			rotation_data._4 += 11520000;
		}

		if (rotation_data._2 >= 0xAFC800)
		{
			rotation_data._2 -= 11520000;
			rotation_data._4 -= 11520000;
		}

		bool rotated = angle >= 2880000 || angle <= -2880000;
		if (rotated)
		{
			if (angle < 0)
			{
				rotation_data._3 -= 2880000;
				if (rotation_data._3 < 0)
				{
					rotation_data._3 += 11520000;
					rotation_data._4 = 0;
				}

			}
			else
			{
				rotation_data._3 += 2880000;
				if (rotation_data._3 >= 11520000)
				{
					rotation_data._3 -= 11520000;
					rotation_data._4 = 0;
				}
			}

			rotation_data._2 = rotation_data._3;
		}

		double rotation = rotation_data._2 * 0.00003125 * ONE_DEGREE_AS_RADIANS;

		forward._1 = (float)sin(rotation);
		forward._2 = (float)cos(rotation);

		rotation += NINETY_DEGREES_AS_RADIANS;

		right._1 = (float)sin(rotation);
		right._2 = (float)cos(rotation);

		if (rotated)
		{
			Float2 Corners[4];

			float SinH = forward._1 * 0.125F;
			float CosH = forward._2 * 0.125F;
			float SinW = forward._1 * 0.250F;
			float CosW = forward._2 * 0.250F;

			Corners[0]._1 = ( CosW + SinH);
			Corners[0]._2 = (-SinW + CosH);

			//1
			Corners[1]._1 = (-CosW + SinH);
			Corners[1]._2 = ( SinW + CosH);

			//2
			Corners[2]._1 = -Corners[0]._1;
			Corners[2]._2 = -Corners[0]._2;

			//3
			Corners[3]._1 = -Corners[1]._1;
			Corners[3]._2 = -Corners[1]._2;

			int quadrant = rotation_data._3 / 2880000;

			player_position._1 += forward._1 * (0.250f - 0.125F);
			player_position._3 += forward._2 * (0.250f - 0.125F);

			test._1 = forward._1 * (0.250f - 0.125F);
			test._2 = forward._2 * (0.250f - 0.125F);

			if (angle < 0)
			{
				player_position._1 -= right._1 * (0.250f - 0.125F);
				player_position._3 -= right._2 * (0.250f - 0.125F);
			}
			else 
			{
				player_position._1 += right._1 * (0.250f - 0.125F);
				player_position._3 += right._2 * (0.250f - 0.125F);
			}
		}

		updated = true;
	}

	if (abs_look > DEAD_ZONE)
	{
		const int RANGE = 65 * 32000;

		rotation_data._1 -= (look_strength * delta_frame * VERTICAL_SPEED);

		if (rotation_data._1 < -RANGE)
			rotation_data._1 = -RANGE;

		if (rotation_data._1 > RANGE)
			rotation_data._1 = RANGE;

		float value = rotation_data._1 * 0.00003125 * ONE_DEGREE_AS_RADIANS;

		up._1 = (float)sin(value);
		up._2 = (float)cos(value);

		updated = true;
	}

	if (!isBlocking)
	{
		Float2 verify = { 0.0f, 0.0f };

		bool moved = false;

		if (abs_forward > DEAD_ZONE)
		{
			float strength = (float)(forward_strength * delta_frame) / 600000L;
			verify._1 += strength * forward._1; //SinX
			verify._2 += strength * forward._2; //CosZ
			moved = true;
		}

		if (abs_right > DEAD_ZONE)
		{
			float strength = (float)(right_strength * delta_frame) / 600000L;
			verify._1 += strength * right._1; //SinX
			verify._2 += strength * right._2; //CosZ
			moved = true;
		}

		if (moved)
		{
			int dir = 0;

			player_position._1 += verify._1;
			player_position._3 += verify._2;

			int angle = rotation_data._2 - rotation_data._3 - rotation_data._4;

			if (angle != 0)
			{
				rotation_data._3 += angle;
				if (rotation_data._3 < 0)
				{
					rotation_data._3 += 11520000;
					rotation_data._4 -= 11520000;
				}
				else if (rotation_data._3 >= 11520000)
				{
					rotation_data._3 -= 11520000;
					rotation_data._4 += 11520000;
				}

				int quadrant = rotation_data._3 / 2880000;

				Float2 Corners[4];

				float SinH = forward._1 * 0.125F;
				float CosH = forward._2 * 0.125F;
				float SinW = forward._1 * 0.250F;
				float CosW = forward._2 * 0.250F;

				Corners[0]._1 = (CosW + SinH);
				Corners[0]._2 = (-SinW + CosH);

				//1
				Corners[1]._1 = (-CosW + SinH);
				Corners[1]._2 = (SinW + CosH);

				//2
				Corners[2]._1 = -Corners[0]._1;
				Corners[2]._2 = -Corners[0]._2;

				//3
				Corners[3]._1 = -Corners[1]._1;
				Corners[3]._2 = -Corners[1]._2;
			}
			updated = true;
		}
	}

	if (player_position._1 < 0.5f)
		player_position._1 = 0.5f;
	else if (player_position._1 > 95.5f)
		player_position._1 = 95.5f;
	if (player_position._3 < 0.5f)
		player_position._3 = 0.5f;
	else if (player_position._3 > 95.5f)
		player_position._3 = 95.5f;

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

	snprintf(buffer, 12, "%f", player_position._1);
	ContentLoader::UpdateOverlayString(13 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", player_position._2);
	ContentLoader::UpdateOverlayString(38 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", player_position._3);
	ContentLoader::UpdateOverlayString(63 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", 0.0f);
	ContentLoader::UpdateOverlayString(89 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", test._1);
	ContentLoader::UpdateOverlayString(115 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", test._2);
	ContentLoader::UpdateOverlayString(138 * 6, buffer, 12 * 6);
}