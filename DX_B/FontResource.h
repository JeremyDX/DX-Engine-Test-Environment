#pragma once

#include "Constants.h"

class FontResource
{
	

	public:

		void CreateGlyphMapping(int mapping_id);
		int32_t MeasureString(const wchar_t * text);
		int32_t MeasureString(const char * text);
		int32_t UpdateBufferString(int vert_offset, const char * text, Vertex32Byte * verts);
		int32_t AddStringToBuffer(const wchar_t * text, Vertex32Byte * verts, Float3 colors, int offset, int draw_offsetX, int draw_offsetY, int alignment);
		int32_t AddStringToBuffer(const char * text, Vertex32Byte * verts, Float3 color, int vert_offset, int draw_offsetX, int draw_offsetY, int alignment);
		signed __int16 glyph_positions[96];

	public:
		unsigned __int16 vertexposition = 0;
		unsigned __int8 string_position = 0;
		unsigned __int8 font_size;
		unsigned __int16 texture_width, texture_height;


};