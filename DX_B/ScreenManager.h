#pragma once

class ScreenManager
{
	public:
		static void UpdateScreenResolution(__int16 resolution_height, __int16 resolution_width);

		static void UpdatePreferredCanvasSize(__int16 height, __int16 width);

		static unsigned __int16 RESOLUTION_WIDTH;
		static unsigned __int16 RESOLUTION_HEIGHT;

		static unsigned __int16 HORIZONTAL_OFFSET;
		static unsigned __int16 VERTICAL_OFFSET;

		static unsigned __int16 PREFERRED_CANVAS_WIDTH;
		static unsigned __int16 PREFERRED_CANVAS_HEIGHT;

		static float ASPECT_RATIO;
};