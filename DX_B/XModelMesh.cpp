#include "pch.h"
#include "XModelMesh.h"

void XModelMesh::CreateTexturedSquare(ID3D11Device *device, ID3D11Buffer** buffer)
{
	VertexPositionTexture square[6]
	{
		{ -1.0F,  1.0F, 0.0f,	0.0F, 0.0F}, //Top Left.
		{  1.0F, -1.0F, 0.0f,	1.0F, 1.0F}, //Bottom Right.
		{ -1.0F, -1.0F, 0.0f,	0.0F, 1.0F}, //Bottom Left.

		{  1.0F, -1.0F, 0.0f,	1.0F, 1.0F}, //Bottom Right.
		{ -1.0F,  1.0F, 0.0f,	0.0F, 0.0F}, //Top Left.
		{  1.0F,  1.0F, 0.0f,	1.0F, 0.0F}, //Top Right.
	};

	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VertexPositionTexture) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { square, 0, 0 };
	device->CreateBuffer(&bd, &srd, buffer);
}

void XModelMesh::DrawString(ID3D11Device *device, FontResource resource, std::string text, float drawX, float drawY)
{
/*	int image_width = 413;
	int image_height = 235;
	int font_size = 38;
	int columns = 16;
	int rows = 6;

	int widths[96]
	{
		16, 27, 41, 64, 87, 111, 135, 145, 159, 173, 216, 237, 248, 263, 274, 291,
		23, 44, 67, 90, 114, 137, 160, 183, 206, 229, 240, 283, 326, 347, 390, 411,
		43, 69, 92, 116, 140, 161, 182, 207, 231, 241, 263, 287, 309, 338, 362,	387,
		23, 48, 71, 95, 119, 143, 168, 202, 227, 252, 276, 290, 307, 321, 364, 384,
		13, 33, 53, 73, 94, 114, 130, 151, 171, 181, 193, 212, 222, 249, 269, 290,
		21, 42, 60, 80, 96, 116, 138, 167, 188, 210, 230, 269, 308, 347, 386, 402
	};

	float a = 2.0F / 775.0F; //0.002500 //Half Screen Width. 
	float b = 2.0F / 454.0F; //0.003333 //Half Screen Height. 

	VertexPositionTexture square[576];

	for (int n = 0; n < 96; ++n)
	{
		int index = n * 6;

		//Vertex4F vertex = resource.GetGlyph(n);
		float left = resource.;
		float right = (widths[n]) / (float)image_width;
		float top = (charHeightPos + 1) / (float)image_height;
		float bottom = (39 + charHeightPos) / (float)image_height;

		square[index + 0] = { nX, pY, 0.0f, left,  top }; //Top Left.
		square[index + 1] = { pX, nY, 0.0f, right, bottom }; //Bottom Right.
		square[index + 2] = { nX, nY, 0.0f, left,  bottom }; //Bottom Left.

		square[index + 3] = { pX, nY, 0.0f, right, bottom }; //Bottom Right.
		square[index + 4] = { nX, pY, 0.0f, left,  top }; //Top Left.
		square[index + 5] = { pX, pY, 0.0f, right, top }; //Top Right.
	}
	// create the vertex buffer
	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VertexPositionTexture) * ARRAYSIZE(square);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { square, 0, 0 };

	device->CreateBuffer(&bd, &srd, buffer);
	*/
}

void XModelMesh::CreateCubeObject(ID3D11Device* device, float xPos, float yPos, float zPos)
{
	// create vertices to represent the corners of the Hypercraft
	VertexPositionTexture OurVertices[] =
	{
		//Texture
		//0,1 - 0,0 - 1,1 - 1-0 Clockwise Starting from Bottom.

		//BACK
		//BR,TR,BL,TL
		{xPos + 1.0f,  yPos + -1.0f, zPos + 1.0f,  0.0f, 1.0f},
		{xPos + 1.0f,  yPos + 1.0f, zPos + 1.0f,  0.0f, 0.0f},
		{xPos + -1.0f,  yPos + -1.0f, zPos + 1.0f,  1.0f, 1.0f},
		{xPos + -1.0f,  yPos + 1.0f, zPos + 1.0f,  1.0f, 0.0f},

		//FRONT
		//BL,TL,BR,TR
		{xPos + -1.0f, yPos + -1.0f, zPos + -1.0f,  0.0f, 1.0f},
		{xPos + -1.0f, yPos + 1.0f, zPos + -1.0f,  0.0f, 0.0f},
		{xPos + 1.0f, yPos + -1.0f, zPos + -1.0f,  1.0f, 1.0f},
		{xPos + 1.0f, yPos + 1.0f, zPos + -1.0f,  1.0f, 0.0f},

		{xPos + -1.0f, yPos + 1.0f, zPos + -1.0f,     0.0f, 0.0f},    // side 3
		{xPos + -1.0f, yPos + 1.0f, zPos + 1.0f,      0.0f, 1.0f},
		{xPos + 1.0f, yPos + 1.0f, zPos + -1.0f,      1.0f, 0.0f},
		{xPos + 1.0f, yPos + 1.0f, zPos + 1.0f,		1.0f, 1.0f},

		{xPos + -1.0f, yPos + -1.0f, zPos + -1.0f,  0.0f, 0.0f},    // side 4
		{xPos + 1.0f,  yPos + -1.0f, zPos + -1.0f,    0.0f, 1.0f},
		{xPos + -1.0f, yPos + -1.0f, zPos + 1.0f,   1.0f, 0.0f},
		{xPos + 1.0f, yPos + -1.0f, zPos + 1.0f,   1.0f, 1.0f},

		{xPos + 1.0f, yPos + -1.0f,zPos + -1.0f,    0.0f, 0.0f},    // side 5
		{xPos + 1.0f, yPos + 1.0f,zPos + -1.0f,  0.0f, 1.0f},
		{xPos + 1.0f, yPos + -1.0f, zPos + 1.0f,     1.0f, 0.0f},
		{xPos + 1.0f, yPos + 1.0f,zPos + 1.0f, 1.0f, 1.0f},

		{xPos + -1.0f, yPos + -1.0f,zPos + -1.0f,  0.0f, 0.0f},    // side 6
		{xPos + -1.0f, yPos + -1.0f,zPos + 1.0f,  0.0f, 1.0f},
		{xPos + -1.0f, yPos + 1.0f,zPos + -1.0f,  1.0f, 0.0f},
		{xPos + -1.0f, yPos + 1.0f,zPos + 1.0f, 1.0f, 1.0f},
	};

	int length = ARRAYSIZE(OurVertices);
	for (int j = 0; j < length; ++j)
		VertexStorage[vertex_count + j] = OurVertices[j];

	int index_position = vertex_count;
	vertex_count += length;

	// create the vertex buffer
	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VertexPositionTexture) * vertex_count;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { VertexStorage, 0, 0 };

	device->CreateBuffer(&bd, &srd, &vertexbuffer);

	// create the index buffer out of shorts
	short OurIndices[] =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 5, 6,    // side 2
		6, 5, 7,
		8, 9, 10,    // side 3
		10, 9, 11,
		12, 13, 14,    // side 4
		14, 13, 15,
		16, 17, 18,    // side 5
		18, 17, 19,
		20, 21, 22,    // side 6
		22, 21, 23,
	};

	length = ARRAYSIZE(OurIndices);
	for (int j = 0; j < length; ++j)
		IndexStorage[index_count + j] = OurIndices[j] + index_position;

	index_count += length;

	// create the index buffer
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.ByteWidth = sizeof(short) * index_count;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA isrd = { IndexStorage, 0, 0 };

	device->CreateBuffer(&ibd, &isrd, &indexbuffer);
}
