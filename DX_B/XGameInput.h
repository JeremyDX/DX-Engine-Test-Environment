#pragma once

#include <Xinput.h>
#pragma comment (lib, "xinput.lib")

struct XBOX_CONTROLLER
{
	public:

		static const uint16

			D_PAD_UP = 0x1,
			D_PAD_DOWN = 0x2,
			D_PAD_RIGHT = 0x4,
			D_PAD_LEFT = 0x8,
			MENU_BUTTON = 0x10,
			VIEW_BUTTON = 0x20,
			LEFT_STICK_CLICK = 0x40,
			RIGHT_STICK_CLICK = 0x80,
			LEFT_BUMPER = 0x100,
			RIGHT_BUMPER = 0x200,
			A_BUTTON = 0x1000,
			B_BUTTON = 0x2000,
			X_BUTTON = 0x4000,
			Y_BUTTON = 0x8000;
};

class XGameInput
{
public:
	static bool LoadController();
	static uint16 GetLeftStickX();
	static uint16 GetLeftStickY();
	static uint16 GetRightStickX();
	static uint16 GetRightStickY();
	static XINPUT_GAMEPAD& GamePad();
	static uint16 GetButtonBitSet();
	static uint32 GetHoldTimeOnBit(int index);
	static int8 GetButtonState(int index);

	//Checks if the "Value" contains the EXACT "Compare" value.
	static bool Compare(int value, int compare);
	static bool Compare(int compare);
};