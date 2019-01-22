#pragma once

#include "Constants.h"

class FontResource
{
	

	public:
		static void SetDrawPosition(int16_t x, int16_t y);
		uint16_t AddDynamicallyToBuffer(Vertex32Byte * buffer, int64_t num, uint16_t offset);
		uint16_t AddDynamicallyToBuffer(Vertex32Byte & buffer, int64_t num, uint16_t offset);
		uint16_t AddDynamicallyToBuffer(int64_t num, uint16_t offset);
		uint16_t AddDynamicallyToBuffer(const char *c, uint16_t offset);

		void CreateGlyphMapping(int mapping_id);
		__int32 MeasureString(const wchar_t * text);
		__int32 MeasureString(const char * text);
		__int32 AddStringToBuffer(const wchar_t * text, Vertex32Byte * verts, Float3 colors, int offset, int draw_offsetX, int draw_offsetY, int alignment);
		__int32 AddStringToBuffer(const char * text, Vertex32Byte * verts, Float3 color, int vert_offset, int draw_offsetX, int draw_offsetY, int alignment);
		signed __int16 glyph_positions[96];

	public:
		unsigned __int16 vertexposition = 0;
		unsigned __int8 string_position = 0;
		unsigned __int8 font_size;
		unsigned __int16 texture_width, texture_height;


};