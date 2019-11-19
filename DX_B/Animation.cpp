#include "pch.h"
#include "Animation.h"

static Animation animations[3];

//Position Translation (Only Support Jumping so only the the Y Axis).
static int16_t* pTranslations;
static int translation_size;

void Animation::CreateJumpAnimation()
{
	pTranslations = new int16_t[40]
	{
		0, 53, 106, 151, 195, 232, 269, 297, 326, 346, 367, 379, 391, 395, 400, 
		395, 391, 379, 367, 346, 326, 297, 269, 232, 195, 151, 106, 53, 0, -61, 
		-115, -102, -89, -76, -64, -51, -38, -25, -12, 0 
	};

	translation_size = 40;
}

int16_t Animation::GetTranslation(int animation_id, int frameIndex)
{
	if (frameIndex >= translation_size)
		return 0;
	return pTranslations[frameIndex];
}

void Animation::Destruct()
{
	if (pTranslations > 0)
		delete[] pTranslations;
	translation_size = 0;
}

void Animation::LoadAnimations()
{
	animations[0].Length = 40;
	animations[1].Length = 40;
	animations[2].Length = 60;
}

Animation & Animation::GetAnimation(int index)
{
	return animations[index];
}

Animation::Animation()
{

}

Animation::~Animation()
{

}
