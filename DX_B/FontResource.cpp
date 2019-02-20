#include "pch.h"
#include "FontResource.h"
#include "ContentLoader.h"
#include "ScreenManager.h"

static int font_incrementer_index = 0;


void FontResource::ResetIncrementer()
{
	font_incrementer_index = 0;
}

//This is hard coded until we can get a Cache file and Process Reading From That...
//TO DO IN FUTURE :)
void FontResource::CreateGlyphMapping(int mapping_id)
{
	font_index = font_incrementer_index++;

	if (mapping_id == 0)
	{
		font_size = 38;
		texture_width = 413;
		texture_height = 235;

		const short positions[96]
		{
			16,27,41,64,87,111,135,145,159,173,216,237,248,263,274,291,
			23,44,67,90,114,137,160,183,206,229,240,283,326,347,390,411,
			43,69,92,116,140,161,182,207,231,241,263,287,309,338,362,387,
			23,48,71,95,119,143,168,202,227,252,276,290,307,321,364,384,
			13,33,53,73,94,114,130,151,171,181,193,212,222,249,269,290,
			21,42,60,80,96,116,138,167,188,210,230,269,308,347,386,402,
		};

		for (int n = 0; n < 96; ++n)
			glyph_positions[n] = positions[n];
	}

	else if (mapping_id == 1)
	{
		font_size = 105;
		texture_width = 1185;
		texture_height = 637;

		const short positions[96]
		{
			49,92,147,227,293,376,458,503,556,608,671,749,794,862,907,965,
			73,124,191,258,331,395,466,535,606,677,720,763,841,919,998,1062,
			92,176,244,322,402,469,535,613,689,733,792,869,937,1023,1100,1184,
			69,160,236,304,384,458,539,637,716,796,869,919,977,1027,1101,1167,
			53,123,194,260,331,398,460,531,598,641,697,768,811,901,968,1040,
			71,142,199,261,319,387,459,549,624,697,766,820,860,914,988,1060,
		};

		for (int n = 0; n < 96; ++n)
			glyph_positions[n] = positions[n];
	}


	else if (mapping_id == 2)
	{

		font_size = 40;
		texture_width = 422;
		texture_height = 247;

		const short positions[96]
		{
			17,30,47,75,103,130,157,170,186,202,221,245,258,276,289,309,
			26,52,78,104,132,158,184,210,236,262,275,288,313,338,362,386,
			28,58,84,112,140,165,190,219,246,258,283,310,334,366,393,421,
			26,55,81,109,135,161,188,223,250,277,303,319,339,355,378,403,
			15,40,65,90,116,140,164,190,216,228,251,276,297,326,352,378,
			25,51,76,101,122,147,171,203,227,252,275,294,306,325,350,367,
		};

		for (int n = 0; n < 96; ++n)
			glyph_positions[n] = positions[n];
	}

	else if (mapping_id == 3)
	{
		font_size = 31;
		texture_width = 348;
		texture_height = 193;

		const short positions[96]
		{
			16,28,43,65,86,108,130,142,157,172,188,208,220,235,247,264,
			21,41,62,83,105,126,147,168,189,210,222,234,254,274,294,313,
			23,46,68,90,113,134,155,178,201,213,233,256,275,301,324,347,
			22,45,67,89,109,131,154,181,202,223,243,258,275,290,309,328,
			12,34,55,76,97,117,137,158,179,191,210,231,250,275,296,317,
			21,42,63,84,101,122,143,168,187,207,226,243,255,272,292,308,
		};

		for (int n = 0; n < 96; ++n)
			glyph_positions[n] = positions[n];
	}
}

int32_t FontResource::MeasureString(const wchar_t *text)
{
	int length = wcslen(text);
	int width = 0;
	for (int j = 0; j < length; ++j)
	{
		int glyphId = ((int)text[j]) - 32;
		int charWidth = glyph_positions[glyphId] - 1;
		if (glyphId % 16 != 0)
			charWidth = glyph_positions[glyphId] - glyph_positions[glyphId - 1] - 1;
		
		width += charWidth;
	}
	return width;
}

int32_t FontResource::MeasureString(const char *text)
{
	int length = strlen(text);
	int width = 0;
	for (int j = 0; j < length; ++j)
	{
		int glyphId = ((int)text[j]) - 32;
		int charWidth = glyph_positions[glyphId] - 1;
		if (glyphId % 16 != 0)
			charWidth = glyph_positions[glyphId] - glyph_positions[glyphId - 1] - 1;
		
		width += charWidth;
	}
	return width;
}

void FontResource::UpdateBufferString(int packed_data, const char *text, static Vertex32Byte *verts)
{
	int packed_position = packed_data >> 6;
	int packed_length = packed_data & 63;

	int length = strlen(text);

	if (length > packed_length)
		length = packed_length;

	float view_width = (float)ScreenManager::PREFERRED_CANVAS_WIDTH;
	float view_height = (float)ScreenManager::PREFERRED_CANVAS_HEIGHT;

	float offsetX = (1.0f + (verts[packed_position]._X)) * (view_width / 2);
	float offsetY = (1.0f - (verts[packed_position]._Y));

	Float3 color = { ContentLoader::CreateShaderColor(1.0f, 0.5f), 1.0f, 1.0f };

	for (int n = 0; n < length; ++n)
	{
		int glyphId = (int)text[n] - 32;

		int charWidth = glyph_positions[glyphId] - 1;
		if (glyphId % 16 != 0)
			charWidth = glyph_positions[glyphId] - glyph_positions[glyphId - 1] - 1;

		float width = (2 * (charWidth)) / view_width;	//Quad Width vs Screen Width.
		float offset = (2 * (offsetX)) / view_width;	//Quad Width vs Screen Width.
		float height = (2 * (font_size)) / view_height;	//Quad Height vs Screen Height.

		float top = 1.0F - offsetY;
		float bottom = 1.0f - height - offsetY;
		float right = -1.0f + width + offset;
		float left = -1.0f + offset;

		float uR = (float)(glyph_positions[glyphId]) / texture_width;
		float uL = (float)(glyph_positions[glyphId] - charWidth) / texture_width;

		float vT = (float)(1 + ((font_size + 1) * (glyphId / 16))) / texture_height;
		float vB = (float)(1 + ((font_size + 1) * (glyphId / 16)) + font_size) / texture_height;

		offsetX += charWidth;

		verts[packed_position++] = { left,  top,    0.0f,	color._1, color._2, color._3,	uL, vT }; //Top Left.
		verts[packed_position++] = { right, bottom, 0.0f,	color._1, color._2, color._3,	uR, vB }; //Bottom Right.
		verts[packed_position++] = { left,  bottom, 0.0f,	color._1, color._2, color._3,	uL, vB }; //Bottom Left.

		verts[packed_position++] = { right, bottom, 0.0f,	color._1, color._2, color._3,	uR, vB }; //Bottom Right.
		verts[packed_position++] = { left,  top,    0.0f,	color._1, color._2, color._3,	uL, vT }; //Top Left.
		verts[packed_position++] = { right, top,    0.0f,	color._1, color._2, color._3,	uR, vT }; //Top Right.
	}

	int clearing_size = 6 * (packed_length - length);

	for (int j = 0; j < clearing_size; ++j)
	{
		verts[packed_position++]._Z = -32768.0f;
	}
}

int32_t FontResource::AddStringToOverlay(const char * text, int padding_skip, Vertex32Byte * verts, ContentOverlay & overlay, int & vertex_offset, Float3 & color, int draw_offsetX, int draw_offsetY)
{
	int length = strlen(text);

	if (length > padding_skip)
		length = padding_skip;

	overlay.Insert((font_index << 21) | (vertex_offset << 6) | padding_skip);

	float view_width = (float)ScreenManager::PREFERRED_CANVAS_WIDTH;
	float view_height = (float)ScreenManager::PREFERRED_CANVAS_HEIGHT;

	int offsetX = draw_offsetX;
	float offsetY = (float)(draw_offsetY * 2) / view_height;

	for (int n = 0; n < length; ++n)
	{
		int glyphId = (int)text[n] - 32;

		int charWidth = glyph_positions[glyphId] - 1;
		if (glyphId % 16 != 0)
			charWidth = glyph_positions[glyphId] - glyph_positions[glyphId - 1] - 1;

		float width = (2 * (charWidth)) / view_width;	//Quad Width vs Screen Width.
		float offset = (2 * (offsetX)) / view_width;	//Quad Width vs Screen Width.
		float height = (2 * (font_size)) / view_height;	//Quad Height vs Screen Height.

		float top = 1.0F - offsetY;
		float bottom = 1.0f - height - offsetY;
		float right = -1.0f + width + offset;
		float left = -1.0f + offset;

		float uR = (float)(glyph_positions[glyphId]) / texture_width;
		float uL = (float)(glyph_positions[glyphId] - charWidth) / texture_width;

		float vT = (float)(1 + ((font_size + 1) * (glyphId / 16))) / texture_height;
		float vB = (float)(1 + ((font_size + 1) * (glyphId / 16)) + font_size) / texture_height;

		offsetX += charWidth;

		verts[vertex_offset++] = { left,  top,    0.0f,	color._1, color._2, color._3,	uL, vT }; //Top Left.
		verts[vertex_offset++] = { right, bottom, 0.0f,	color._1, color._2, color._3,	uR, vB }; //Bottom Right.
		verts[vertex_offset++] = { left,  bottom, 0.0f,	color._1, color._2, color._3,	uL, vB }; //Bottom Left.

		verts[vertex_offset++] = { right, bottom, 0.0f,	color._1, color._2, color._3,	uR, vB }; //Bottom Right.
		verts[vertex_offset++] = { left,  top,    0.0f,	color._1, color._2, color._3,	uL, vT }; //Top Left.
		verts[vertex_offset++] = { right, top,    0.0f,	color._1, color._2, color._3,	uR, vT }; //Top Right.
	}

	int clearing_size = 6 * (padding_skip - length);

	if (clearing_size > 0)
	{
		verts[vertex_offset]._X = -1.0f + ((2 * (offsetX)) / view_width);
		verts[vertex_offset]._Y =  1.0F - offsetY;

		for (int j = 0; j < clearing_size; ++j)
		{
			verts[vertex_offset++]._Z = -32768.0f;
		}
	}

	return offsetX;
}

int32_t FontResource::AddStringToBuffer(const char *text, Vertex32Byte *verts, Float3 &color, int vert_offset, int draw_offsetX, int draw_offsetY)
{
	int length = strlen(text);
	float view_width = (float)ScreenManager::PREFERRED_CANVAS_WIDTH;
	float view_height = (float)ScreenManager::PREFERRED_CANVAS_HEIGHT;

	int stringWidth = MeasureString(text);

    int offsetX = draw_offsetX;
	float offsetY = (float)(draw_offsetY * 2) / view_height;

	for (int n = 0; n < length; ++n)
	{

		int glyphId = (int)text[n] - 32;
		if (glyphId == -22)
		{
			offsetY += (2 * (font_size)) / view_height;
			offsetX = 0;
			continue;
		}

		int charWidth = glyph_positions[glyphId] - 1;
		if (glyphId % 16 != 0)
			charWidth = glyph_positions[glyphId] - glyph_positions[glyphId - 1] - 1;

		float width  = (2 * (charWidth)) / view_width;	//Quad Width vs Screen Width.
		float offset = (2 * (offsetX))  / view_width;	//Quad Width vs Screen Width.
		float height = (2 * (font_size)) / view_height;	//Quad Height vs Screen Height.

		float top	 =  1.0F - offsetY;
		float bottom =  1.0f - height - offsetY;
		float right  = -1.0f + width + offset;
		float left   = -1.0f + offset;

		float uR = (float)(glyph_positions[glyphId]) / texture_width;
		float uL = (float)(glyph_positions[glyphId] - charWidth) / texture_width;

		float vT = (float)(1 + ((font_size + 1) * (glyphId / 16))) / texture_height;
		float vB = (float)(1 + ((font_size + 1) * (glyphId / 16)) + font_size) / texture_height;

		offsetX += charWidth;

		verts[vert_offset++] = { left,  top,    0.0f,	color._1, color._2, color._3,	uL, vT }; //Top Left.
		verts[vert_offset++] = { right, bottom, 0.0f,	color._1, color._2, color._3,	uR, vB }; //Bottom Right.
		verts[vert_offset++] = { left,  bottom, 0.0f,	color._1, color._2, color._3,	uL, vB }; //Bottom Left.

		verts[vert_offset++] = { right, bottom, 0.0f,	color._1, color._2, color._3,	uR, vB }; //Bottom Right.
		verts[vert_offset++] = { left,  top,    0.0f,	color._1, color._2, color._3,	uL, vT }; //Top Left.
		verts[vert_offset++] = { right, top,    0.0f,	color._1, color._2, color._3,	uR, vT }; //Top Right.
	}
	return vert_offset;
}
