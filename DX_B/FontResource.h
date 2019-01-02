#pragma once

#include <iostream>
#include <sstream>
#include <string.h>
#include "VertexStructureTypes.h"

using namespace std;

class FontResource
{
	

	public:
		void CreateGlyphMapping(int mapping_id);
		__int32 MeasureString(const wchar_t * text);
		__int32 MeasureString(const char * text);
		__int32 AddStringToBuffer(const wchar_t * text, VertexPositionColorTexture * verts, Vertex3F colors, int offset, int draw_offsetX, int draw_offsetY, int alignment);
		__int32 AddStringToBuffer(const char * text, VertexPositionColorTexture * verts, Vertex3F color, int vert_offset, int draw_offsetX, int draw_offsetY, int alignment);
		__int32 CreateStringText(const char * text, float drawX, float drawY);
		signed __int16 glyph_positions[96];

	public:
		unsigned __int16 vertexposition = 0;
		unsigned __int8 string_position = 0;
		unsigned __int8 font_size;
		unsigned __int16 texture_width, texture_height;


};