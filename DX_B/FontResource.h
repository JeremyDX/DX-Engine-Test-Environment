#pragma once

#include "Constants.h"
#include "ContentOverlay.h"

class FontResource
{
	public:
		static void ResetIncrementer();
		void CreateGlyphMapping(int mapping_id);
		int32_t MeasureString(const wchar_t * text);
		int32_t MeasureString(const char * text);
		void UpdateBufferString(int vert_offset, const char * text, Vertex32Byte * verts);
		int32_t AddStringToOverlay(const char * text, int padding_skip, Vertex32Byte * verts, ContentOverlay & overlay, int & vertex_offset, Float3 & color, int draw_offsetX, int draw_offsetY);
		int32_t AddStringToBuffer(const char * text, Vertex32Byte * verts, Float3 &color, int vert_offset, int draw_offsetX, int draw_offsetY);
		int16_t glyph_positions[96];
		uint8_t font_index;

	private:
		uint16_t texture_width, texture_height;
		uint8_t font_size;
};