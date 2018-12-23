#include "pch.h"
#include "FontResource.h"

//This is hard coded until we can get a Cache file and Process Reading From That...
//TO DO IN FUTURE :)
void FontResource::CreateGlyphMapping()
{
	int widths[96] //6*16 = 96..
	{
		16, 27, 41, 64, 87, 111, 135, 145, 159, 173, 216, 237, 248, 263, 274, 291,
		23, 44, 67, 90, 114, 137, 160, 183, 206, 229, 240, 283, 326, 347, 390, 411,
		43, 69, 92, 116, 140, 161, 182, 207, 231, 241, 263, 287, 309, 338, 362,	387,
		23, 48, 71, 95, 119, 143, 168, 202, 227, 252, 276, 290, 307, 321, 364, 384,
		13, 33, 53, 73, 94, 114, 130, 151, 171, 181, 193, 212, 222, 249, 269, 290,
		21, 42, 60, 80, 96, 116, 138, 167, 188, 210, 230, 269, 308, 347, 386, 402
	};

	float image_width = 413.0F;
	float image_height = 235.0F;

	for (int n = 0; n < 96; ++n)
	{
		int charWidth = n % 16;
		if (charWidth == 0)
			charWidth = widths[n] - 1;
		else //27-16 = 11.
			charWidth = widths[n] - widths[n - 1] - 1;

		int charHeightPos = 39 * (int)(n / 16);

		//LEFT mapping.
		glyph_mapped_uvs[n].R = (widths[n] - charWidth) / image_width;
		
		//RIGHT mapping.
		glyph_mapped_uvs[n].G = (widths[n] - 1) / image_width;
		
		//TOP mapping.
		glyph_mapped_uvs[n].B = (charHeightPos + 1) / image_height;
		//40 , X = 39 + 1 = 40.
		//77 , X = 39 + 38 = 77.
		//BOTTOM mapping.
		glyph_mapped_uvs[n].A = (38 + charHeightPos) / image_height;
	}
}

void FontResource::AddStringToBuffer(D3D11_VIEWPORT view, const wchar_t *text, int drawX, int drawY)
{
	int widths[96]
	{
		16, 27, 41, 64, 87, 111, 135, 145, 159, 173, 216, 237, 248, 263, 274, 291,
		23, 44, 67, 90, 114, 137, 160, 183, 206, 229, 240, 283, 326, 347, 390, 411,
		43, 69, 92, 116, 140, 161, 182, 207, 231, 241, 263, 287, 309, 338, 362,	387,
		23, 48, 71, 95, 119, 143, 168, 202, 227, 252, 276, 290, 307, 321, 364, 384,
		13, 33, 53, 73, 94, 114, 130, 151, 171, 181, 193, 212, 222, 249, 269, 290,
		21, 42, 60, 80, 96, 116, 138, 167, 188, 210, 230, 269, 308, 347, 386, 402
	};

	int length = wcslen(text);

	float a = 2.0F / view.Width;
	float b = 2.0F / view.Height;

	float offset_width = a * drawX;
	float offset_height = b * drawY;

	float left = -1.0F + offset_width;
	float top = 1.0F - offset_height;

	float right = left; //updated with char values.
	float bottom = top; //updated with char values.

	for (int i = 0; i < length; ++i)
	{

		int character = (int)text[i] - 32;
		Vertex4F v = glyph_mapped_uvs[character];

		right += v.G - v.R; //Set Right Draw Position.
		bottom = top - (v.A - v.B); //Set Bottom Draw Position.

		vertexstorage[vertexposition++] = { left, top, 0.0f, v.R, v.B }; //Left Top.   R, B
		vertexstorage[vertexposition++] = { right, bottom, 0.0f, v.G, v.A }; //Right Bottom. G, A
		vertexstorage[vertexposition++] = { left, bottom, 0.0f, v.R, v.A }; //Left Bottom. R, A

		vertexstorage[vertexposition++] = { right, bottom, 0.0f, v.G, v.A }; //Right Bottom. G, A
		vertexstorage[vertexposition++] = { left, top, 0.0f, v.R, v.B }; //Left Top. R, B
		vertexstorage[vertexposition++] = { right, top, 0.0f, v.G, v.B }; //Right Top. G, B

		left = right;
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

int FontResource::GetSize()
{
	return vertexposition * 6;
}
