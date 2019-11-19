#pragma once

#include "Constants.h"

class Animation
{
	public:
		static void CreateJumpAnimation();
		static void LoadAnimations();
		static Animation& GetAnimation(int index);
		Animation();
		~Animation();

		static int16_t GetTranslation(int animation_id, int frameIndex);
		static void Destruct();

	public:
		uint8_t Length;
};