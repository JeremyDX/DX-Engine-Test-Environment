#include "pch.h"
#include "ScreenManager.h"

static __int32 UI_SCALE = 9;

__int32 ScreenManager::GetUIScaleWidth()
{
	return UI_SCALE * 90 + 270;
}

__int32 ScreenManager::GetUIScaleHeight()
{
	return UI_SCALE * 160 + 480;
}

