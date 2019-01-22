#include "pch.h"
#include "ScreenManager.h"

uint16_t ScreenManager::RESOLUTION_WIDTH;
uint16_t ScreenManager::RESOLUTION_HEIGHT;
           
uint16_t ScreenManager::HORIZONTAL_OFFSET = 0;
uint16_t ScreenManager::VERTICAL_OFFSET = 0;

uint16_t ScreenManager::PREFERRED_CANVAS_WIDTH = 0;
uint16_t ScreenManager::PREFERRED_CANVAS_HEIGHT = 0;

   float ScreenManager::ASPECT_RATIO;

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