#include "pch.h"
#include "FontResource.h"

#include <inttypes.h>

static char* buffer = NULL;

void FontResource::AddIntegerToBuffer(int64_t value, int offset = 0)
{
	int64_t sign = -(value < 0);
	value = (value ^ sign) - sign;

	int len = 0;

	if (value >= 1000000000) //10 Digit.
	{
		if (value >= 100000000000000) //15 Digit.
		{
			if (value >= 10000000000000000) //17 Digits.
			{
				if (value >= 1000000000000000000)
					len = 19;
				else
				{
					if (value >= 100000000000000000)
						len = 18;
					else
						len = 17;
				}
			}
			else
			{
				if (value >= 1000000000000000)
					len = 16;
				else
				{
					len = 15;
				}
			}
		}
		else {
			if (value >= 100000000000) // 12 Digits.
			{
				if (value >= 10000000000000) //14 Digits.
					len = 14;
				else
				{
					if (value >= 1000000000000) //13 Digits.
						len = 13;
					else
						len = 12; //12 Digits.
				}
			}
			else
			{
				if (value >= 10000000000)
					len = 11; //11 Digits.
				else
				{
					len = 10; //10 Digits.
				}
			}
		}
	}
	else {
		if (value >= 100000) //6 Digit.
		{
			if (value >= 10000000)
			{
				if (value >= 100000000)
					len = 9;
				else
					len = 8;
			}
			else
			{
				if (value >= 1000000)
					len = 7;
				else
					len = 6;
			}
		}
		else {
			if (value >= 1000)
			{
				if (value >= 10000)
					len = 5;
				else
					len = 4;
			}
			else
			{
				if (value >= 100)
					len = 3;
				else
				{
					if (value >= 10)
						len = 2;
					else
						len = 1;
				}
			}
		}
	}

	if (sign)
	{
		buffer[0] = '-';
		++len;
	}

	buffer[offset + len] = 0;
	while (--len >= 0)
	{
		uint64_t c_value = (uint64_t)(value * 0.1);
		buffer[offset + len] = (char)(48 + (value - c_value * 10));
		value = c_value;
	}
}