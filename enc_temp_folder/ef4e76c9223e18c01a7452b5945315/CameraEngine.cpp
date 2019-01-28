#include "pch.h"
#include "CameraEngine.h"
#include "XGameInput.h"
#include "GameTime.h"
#include "Animation.h"
#include "ScreenManager.h"
#include "ContentLoader.h"

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

Float3 blocking_value[4];

int forced_animation_index = 0;
Float4 anim_move_vectors = { 0.0f, 0.0f, 0.0f, 0.0f };
Float3 base_animation_position = { 0.0f, 0.0f, 0.0f };
Int2 animation_move_strength = { 0, 0 };
bool throttled_jump = false;
bool sprinting = false;

int HORIZONTAL_SPEED = 4;
int VERTICAL_SPEED = 4;

int face_quadrant = 0;
int move_quadrant = 0;
int move_direction = 0;
int blocking_quadrant = 0;


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

	float height = position._2 + 2.0f;

	//Dot Product (Position, Right Vector).
	camera_matrix._41 = -(position._1 * camera_matrix._11 + height * camera_matrix._21 + position._3 * camera_matrix._31);

	//Dot Product (Position, Up Vector).
	camera_matrix._42 = -(position._1 * camera_matrix._12 + height * camera_matrix._22 + position._3 * camera_matrix._32);

	//Dot Product (Position, Forward Vector).
	camera_matrix._43 = -(position._1 * camera_matrix._13 + height * camera_matrix._23 + position._3 * camera_matrix._33);
}

void CreateFinalMatrixResult()
{
	static const XMMATRIX PROJECTION_MATRIX = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), ScreenManager::ASPECT_RATIO, 1, 1000);
	DirectX::XMStoreFloat4x4(&CameraEngine::final_result, DirectX::XMLoadFloat4x4(&camera_matrix) * PROJECTION_MATRIX);
}

void CameraEngine::ResetPrimaryCameraMatrix()
{
	position._1 = 0.5F;
	position._2 = 0.0F;
	position._3 = 0.5F;

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
			anim_move_vectors._2 = right._3;
			anim_move_vectors._3 = forward._1;
			anim_move_vectors._4 = forward._3;
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

	//If the player did in fact turn updated the camera movement vectors. 
	if (abs_turn > dead_zone)
	{
		rotation_data._2 += (turn_strength * (long)GameTime::DeltaTime_Frames() * HORIZONTAL_SPEED);
		//rotation.y = 32768 * 10;

		//0xB40000 = 360 * 32768 in hex form.
		if (rotation_data._2 < 0)
			rotation_data._2 += 0xB40000;
		if (rotation_data._2 >= 0xB40000)
			rotation_data._2 -= 0xB40000;

		float value = rotation_data._2 / 32768.0F;
		double PI = 3.14159265;

		int f = (int)value / 90;
		blocking_value[0]._3 = value;
		
		if (f == 0)
		{
			float R = (value - 0.0f) / 90.0f;
			float R2 = 1.0F - R;
			blocking_value[0]._1 = R2 * -0.21875F +  R *  0.125F;
			blocking_value[0]._2 = R  *  0.21875F - R2 * -0.125F;

			blocking_value[1]._1 = R2 * 0.21875F - R * -0.125F;
			blocking_value[1]._2 = R * -0.21875F + R2 * 0.125F;
		}
		else if (f == 1)
		{
			float R = (value - 90.0f) / 90.0f;
			float R2 = 1.0F - R;
			blocking_value[0]._1 = R *  0.21875F - R2 * -0.125F;
			blocking_value[0]._2 = R2 * 0.21875F + R *  -0.125F;

			blocking_value[1]._1 = R * -0.21875F + R2 * 0.125F;
			blocking_value[1]._2 = R2 * -0.21875F - R * 0.125F;
		}
		else if (f == 2)
		{
			float R = (value - 180.0f) / 90.0f;
			float R2 = 1.0F - R;

			blocking_value[0]._1 = R2 * 0.21875F + R * -0.125F;
			blocking_value[0]._2 = R * -0.21875F - R2 * 0.125F;

			blocking_value[1]._1 = R2 * -0.21875F - R * 0.125F;
			blocking_value[1]._2 = R *  0.21875F + R2 * -0.125F;
		}
		else if (f == 3)
		{
			float R = (value - 270.0f) / 90.0f;
			float R2 = 1.0F - R;
			blocking_value[0]._1 =  R * -0.21875F - R2 * 0.125F;
			blocking_value[0]._2 = R2 * -0.21875F + R * 0.125F;

			blocking_value[1]._1 = R *  0.21875F + R2 * -0.125F;
			blocking_value[1]._2 = R2 *  0.21875F - R * -0.125F;
		}

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
		rotation_data._1 -= (look_strength * (long)GameTime::DeltaTime_Frames() * VERTICAL_SPEED);
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

	if (!isBlocking)
	{
		int move_x = 1;
		int move_z = 3;

		Float2 verify = { 0.0f, 0.0f };

		bool mov_z = false;
		bool mov_x = false;

		if (abs_forward > dead_zone)
		{
			float strength = (float)(forward_strength * (long)GameTime::DeltaTime_Frames()) / 600000L;
			verify._1 += strength * forward._1;
			verify._2 += strength * forward._3;
		}

		if (abs_right > dead_zone)
		{
			float strength = (float)(right_strength * (long)GameTime::DeltaTime_Frames()) / 600000L;
			verify._1 += strength * right._1;
			verify._2 += strength * right._3;
		}
		
		move_quadrant = 4;

		if (verify._1 > 0.0f)
			++move_quadrant;
		else if (verify._1 < 0.0f)
			--move_quadrant;

		if (verify._2 > 0.0f)
			move_quadrant -= 3;
		else if (verify._2 < 0.0f)
			move_quadrant += 3;

		if (move_quadrant != 4)
		{
			updated = true;

			move_direction = 4;
			
			if (abs_forward > dead_zone)
			{
				if (forward_strength > 0)
					move_direction -= 3;
				else
					move_direction += 3;
			}
			if (abs_right > dead_zone)
			{
				if (right_strength > 0)
					++move_direction;
				else
					--move_direction;
			}
		}

		position._1 += verify._1;
		position._3 += verify._2;
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

	snprintf(buffer, 12, "%f", blocking_value[1]._1);
	ContentLoader::UpdateOverlayString(89 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", blocking_value[1]._2);
	ContentLoader::UpdateOverlayString(115 * 6, buffer, 12 * 6);

	snprintf(buffer, 12, "%f", blocking_value[0]._3);
	ContentLoader::UpdateOverlayString(138 * 6, buffer, 12 * 6);
}