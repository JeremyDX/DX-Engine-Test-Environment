#include "pch.h"
#include "FontResource.h"

//This is hard coded until we can get a Cache file and Process Reading From That...
//TO DO IN FUTURE :)
void FontResource::CreateGlyphMapping()
{
	short positions[96]
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

void FontResource::AddStringToBuffer(D3D11_VIEWPORT view, const wchar_t *text, int drawX, int drawY)
{
	int font_size = 38;

	int length = wcslen(text);

	int offset_x = 0;
	float offsetY = 0.0F;

	for (int n = 0; n < length; ++n)
	{

		int glyphId = (int)text[n] - 32;
		if (glyphId == -22)
		{
			offsetY += (2 * (font_size)) / view.Height;
			offset_x = 0;
			continue;
		}

		int charWidth = glyph_positions[glyphId] - 1;
		if (glyphId % 16 != 0)
			charWidth = glyph_positions[glyphId] - glyph_positions[glyphId - 1] - 1;

		float width = (2 * (charWidth)) / view.Width;	//Quad Width vs Screen Width.
		float offset = (2 * (offset_x)) / view.Width;	//Quad Width vs Screen Width.
		float height = (2 * (font_size)) / view.Height;	//Quad Height vs Screen Height.

		float top	 =  1.0F - offsetY;
		float bottom =  1.0f - height - offsetY;
		float right  = -1.0f + width + offset;
		float left   = -1.0f + offset;

		float uR = (float)glyph_positions[glyphId] / 413;
		float uL = (float)(glyph_positions[glyphId] - charWidth) / 413;

		float vT = (float)(1 + ((font_size + 1) * (glyphId / 16))) / 235;
		float vB = (float)(1 + ((font_size + 1) * (glyphId / 16)) + font_size) / 235;

		offset_x += charWidth;

		vertexstorage[vertexposition++] = { left,  top,   0.0f,	uL, vT }; //Top Left.
		vertexstorage[vertexposition++] = { right, bottom, 0.0f,	uR, vB }; //Bottom Right.
		vertexstorage[vertexposition++] = { left,  bottom, 0.0f,	uL, vB }; //Bottom Left.

		vertexstorage[vertexposition++] = { right, bottom, 0.0f,	uR, vB }; //Bottom Right.
		vertexstorage[vertexposition++] = { left,  top,   0.0f,	uL, vT }; //Top Left.
		vertexstorage[vertexposition++] = { right, top,   0.0f,	uR, vT }; //Top Right.
	}
}

void FontResource::PushStringsToDeviceBuffer(ID3D11Device *device)
{
	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VertexPositionTexture) * vertexposition;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { vertexstorage, 0, 0 };

	device->CreateBuffer(&bd, &srd, &buffer);
}

void FontResource::PushStringsToDeviceBuffer(ID3D11Device *device , ID3D11Buffer **vertexbuffer)
{
	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VertexPositionTexture) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { vertexstorage, 0, 0 };

	device->CreateBuffer(&bd, &srd, vertexbuffer);
}

int FontResource::GetSize()
{
	return vertexposition * 6;
}
