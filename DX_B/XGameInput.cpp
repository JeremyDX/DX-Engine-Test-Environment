#include "pch.h"

#include "XGameInput.h"
#include "GameTime.h"

XINPUT_GAMEPAD gamepad;

uint16_t BUTTONS_HOLD = 0;
uint16_t BUTTONS_PRESSED = 0;
uint16_t BUTTONS_RELEASED = 0;

uint16_t HOLD_TRACKING = 0;
uint64_t TRACKING_BEGIN = 0;

uint32_t CURRENT = 0;

bool XGameInput::LoadController()
{
	uint64_t TIME_STAMP = GameTime::AbsoluteFrameTicks();

	XINPUT_STATE xstate;
	ZeroMemory(&xstate, sizeof(XINPUT_STATE));

	if (XInputGetState(0, &xstate) == 0L)
	{
		gamepad = xstate.Gamepad;

		uint64_t LAST = CURRENT;

		CURRENT = gamepad.wButtons;

		BUTTONS_HOLD = LAST & CURRENT;
		BUTTONS_RELEASED = LAST ^ BUTTONS_HOLD;
		BUTTONS_PRESSED = CURRENT ^ BUTTONS_HOLD;

		if((BUTTONS_HOLD & HOLD_TRACKING) != HOLD_TRACKING)
			TRACKING_BEGIN = GameTime::AbsoluteFrameTicks();

		return 1;
	}
	return 0;
}

uint64_t XGameInput::GetTrackedHoldTime(uint16_t value)
{
	HOLD_TRACKING = value;
	return GameTime::AbsoluteFrameTicks() - TRACKING_BEGIN;
}

void XGameInput::ResetHoldTracking()
{
	HOLD_TRACKING = XBOX_CONTROLLER::ALL_BUTTONS;
	TRACKING_BEGIN = GameTime::AbsoluteFrameTicks();
}

uint16 XGameInput::GetLeftStickX()
{
	return gamepad.sThumbLX;
}

uint16 XGameInput::GetLeftStickY()
{
	return gamepad.sThumbLY;
}

uint16 XGameInput::GetRightStickX()
{
	return gamepad.sThumbRX;
}

uint16 XGameInput::GetRightStickY()
{
	return gamepad.sThumbRY;
}

XINPUT_GAMEPAD& XGameInput::GamePad()
{
	return gamepad;
}

uint16 XGameInput::GetButtonBitSet()
{
	return gamepad.wButtons;
}

bool XGameInput::ExactButtonsPressed(int value)
{
	return (BUTTONS_PRESSED & value) == value;
}

bool XGameInput::ExactButtonsReleased(int value)
{
	return (BUTTONS_RELEASED & value) == value;
}

bool XGameInput::ExactButtonsHolding(int value)
{
	return (BUTTONS_HOLD & value) == value;
}

uint16_t XGameInput::AnyButtonsPressed(int value)
{
	return BUTTONS_PRESSED & value;
}

uint16_t XGameInput::AnyButtonsReleased(int value)
{
	return BUTTONS_RELEASED & value;
}

uint16_t XGameInput::AnyButtonsHolding(int value)
{
	return BUTTONS_HOLD & value;
}
