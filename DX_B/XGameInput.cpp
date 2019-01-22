#include "pch.h"

#include "XGameInput.h"
#include "GameTime.h"

XINPUT_STATE gamepad_state;

uint16_t BUTTONS_HOLD = 0;
uint16_t BUTTONS_PRESSED = 0;
uint16_t BUTTONS_RELEASED = 0;

uint16_t HOLD_TRACKING = 0;

uint64_t TRACKING_BEGIN = 0;

bool XGameInput::LoadController()
{
	uint16_t LAST = gamepad_state.Gamepad.wButtons;
	
	ZeroMemory(&gamepad_state, sizeof(XINPUT_STATE));

	if (XInputGetState(0, &gamepad_state) == 0L)
	{
		uint16_t CURRENT = gamepad_state.Gamepad.wButtons;

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
	return gamepad_state.Gamepad.sThumbLX;
}

uint16 XGameInput::GetLeftStickY()
{
	return gamepad_state.Gamepad.sThumbLY;
}

uint16 XGameInput::GetRightStickX()
{
	return gamepad_state.Gamepad.sThumbRX;
}

uint16 XGameInput::GetRightStickY()
{
	return gamepad_state.Gamepad.sThumbRY;
}

XINPUT_GAMEPAD& XGameInput::GamePad()
{
	return gamepad_state.Gamepad;
}

uint16 XGameInput::GetButtonBitSet()
{
	return gamepad_state.Gamepad.wButtons;
}

bool XGameInput::AllOfTheseButtonsArePressed(int value)
{
	return (BUTTONS_PRESSED & value) == value;
}

bool XGameInput::AllOfTheseButtonsAreReleased(int value)
{
	return (BUTTONS_RELEASED & value) == value;
}

bool XGameInput::AllOfTheseButtonsAreHolding(int value)
{
	return (BUTTONS_HOLD & value) == value;
}

uint16_t XGameInput::AnyOfTheseButtonsArePressed(int value)
{
	return BUTTONS_PRESSED & value;
}

uint16_t XGameInput::AnyOfTheseButtonsAreReleased(int value)
{
	return BUTTONS_RELEASED & value;
}

uint16_t XGameInput::AnyOfTheseButtonsAreHolding(int value)
{
	return BUTTONS_HOLD & value;
}

bool XGameInput::AnyButtonPressed()
{
	return BUTTONS_PRESSED != 0;
}

bool XGameInput::AnyButtonReleased()
{
	return BUTTONS_PRESSED != 0;
}

bool XGameInput::AnyButtonHeld()
{
	return BUTTONS_HOLD != 0;
}
