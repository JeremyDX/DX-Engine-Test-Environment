#pragma once

#include <Xinput.h>

struct XBOX_CONTROLLER
{
	public:

		static const uint16_t

			D_PAD_UP = 0x1,
			D_PAD_DOWN = 0x2,
			D_PAD_LEFT = 0x4,
			D_PAD_RIGHT = 0x8,
			MENU_BUTTON = 0x10,
			VIEW_BUTTON = 0x20,
			LEFT_STICK_CLICK = 0x40,
			RIGHT_STICK_CLICK = 0x80,
			LEFT_BUMPER = 0x100,
			RIGHT_BUMPER = 0x200,
			A_BUTTON = 0x1000,
			B_BUTTON = 0x2000,
			X_BUTTON = 0x4000,
			Y_BUTTON = 0x8000,
		 ALL_BUTTONS = 0xF3FE;
};

class XGameInput
{
public:

	static bool LoadController();

	static uint64_t GetTrackedHoldTime(uint16_t value = XBOX_CONTROLLER::ALL_BUTTONS);
	static void ResetHoldTracking();
	static uint16_t GetLeftStickX();
	static uint16_t GetLeftStickY();
	static uint16_t GetRightStickX();
	static uint16_t GetRightStickY();
	static XINPUT_GAMEPAD& GamePad();
	static uint16 GetButtonBitSet();

	static bool AllOfTheseButtonsArePressed(int value);
	static bool AllOfTheseButtonsAreReleased(int value);
	static bool AllOfTheseButtonsAreHolding(int value);

	static uint16_t AnyOfTheseButtonsArePressed(int value = XBOX_CONTROLLER::ALL_BUTTONS);
	static uint16_t AnyOfTheseButtonsAreReleased(int value = XBOX_CONTROLLER::ALL_BUTTONS);
	static uint16_t AnyOfTheseButtonsAreHolding(int value = XBOX_CONTROLLER::ALL_BUTTONS);

	static bool AnyButtonPressed();
	static bool AnyButtonReleased();
	static bool AnyButtonHeld();
};

