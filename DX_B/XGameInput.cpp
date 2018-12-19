#include "pch.h"

#include "XGameInput.h"

XINPUT_GAMEPAD lastpad;
XINPUT_GAMEPAD gamepad;

uint32 holdTimes[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// 0 = Inactive Buttton, 1 = Button is Released, 2 = Button is Presssed, 4 = Button is Held.
int8 buttonstate[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


bool XGameInput::LoadController()
{
	XINPUT_STATE state;

	ZeroMemory(&state, sizeof(XINPUT_STATE));
	ZeroMemory(&buttonstate, sizeof(buttonstate));

	if (XInputGetState(0, &state) == 0L)
	{
		lastpad = gamepad;
		gamepad = state.Gamepad;

		for (int i = 0; i < 16; ++i)
		{
			if ((lastpad.wButtons & (1 << i)) == (1 << i))
			{
				///Last Frame Button Was Pressed.
				if ((gamepad.wButtons & (1 << i)) == (1 << i))
				{
					++holdTimes[i];
					///Current Frame Button Was Pressed, Button is being Held.
					buttonstate[i] |= 4;
				}
				else {
					///Current Frame Button is Released.
					buttonstate[i] |= 1;
				}
			}
			else {
				if ((gamepad.wButtons & (1 << i)) == (1 << i))
				{
					holdTimes[i] = 0; ///Reset The Held Time When A New Occurance Is Pressed.
					buttonstate[i] |= 2;
				}
			}
		}

		return 1;
	}
	return 0;
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

uint32 XGameInput::GetHoldTimeOnBit(int index)
{
	return holdTimes[index];
}

int8 XGameInput::GetButtonState(int index)
{
	return buttonstate[index];
}

bool XGameInput::Compare(int value, int compare)
{
	if ((value & compare) == compare)
		return 0x1;
	return 0x0;
}

bool XGameInput::Compare(int compare)
{
	if ((gamepad.wButtons & compare) == compare)
		return 0x1;
	return 0x0;
}
