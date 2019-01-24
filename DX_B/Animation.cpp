#include "pch.h"
#include "Animation.h"

static Animation animations[3];

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
