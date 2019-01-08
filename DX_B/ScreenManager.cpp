#include "pch.h"
#include "ScreenManager.h"

unsigned __int16 ScreenManager::RESOLUTION_WIDTH;
unsigned __int16 ScreenManager::RESOLUTION_HEIGHT;
           float ScreenManager::ASPECT_RATIO;

unsigned __int16 ScreenManager::HORIZONTAL_OFFSET = 0;
unsigned __int16 ScreenManager::VERTICAL_OFFSET = 0;

unsigned __int16 ScreenManager::PREFERRED_CANVAS_WIDTH = 0;
unsigned __int16 ScreenManager::PREFERRED_CANVAS_HEIGHT = 0;

void ScreenManager::UpdateScreenResolution(__int16 resolution_height, __int16 resolution_width)
{
	RESOLUTION_WIDTH = resolution_width;
	RESOLUTION_HEIGHT = resolution_height;

	if (PREFERRED_CANVAS_WIDTH == 0)
		PREFERRED_CANVAS_WIDTH = resolution_width;

	if (PREFERRED_CANVAS_HEIGHT == 0)
		PREFERRED_CANVAS_HEIGHT = resolution_height;

	ASPECT_RATIO = (float)resolution_width / (float)resolution_height;
}

void ScreenManager::UpdatePreferredCanvasSize(__int16 height, __int16 width)
{
	PREFERRED_CANVAS_WIDTH = width;
	PREFERRED_CANVAS_HEIGHT = height;
}
