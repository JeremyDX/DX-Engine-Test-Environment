#pragma once

#include "Constants.h"

class Animation
{
	public:
		static void LoadAnimations();
		static Animation& GetAnimation(int index);
		Animation();
		~Animation();

	public:
		uint8_t Length;
};