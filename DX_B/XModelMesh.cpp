#include "pch.h"
#include "XModelMesh.h"

void XModelMesh::CreateTexturedSquare(ID3D11Device *device, ID3D11Buffer** buffer, float drawX, float drawY, TextureResource resource)
{
	float a = 1.0F / 400.0F; //0.002500 //Half Screen Width. 
	float b = 1.0F / 300.0F; //0.003333 //Half Screen Height. 

	float nX = -1.0F + (drawX * a);
	float nY = 1.0F - ((drawY + 512) * b);

	float pX = -1.0F + ((drawX + 512) * a);
	float pY = 1.0F - (drawY * b);

	VertexPositionTexture square[6]
	{									
		{ nX,  pY, 0.0f,	0.0F, 0.0F}, //Top Left.
		{ pX,  nY, 0.0f,	1.0F, 1.0F}, //Bottom Right.
		{ nX,  nY, 0.0f,	0.0F, 1.0F}, //Bottom Left.

		{ pX,  nY, 0.0f,	1.0F, 1.0F}, //Bottom Right.
		{ nX,  pY, 0.0f,	0.0F, 0.0F}, //Top Left.
		{ pX,  pY, 0.0f,	1.0F, 0.0F}, //Top Right.
	};

	// create the vertex buffer
	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VertexPositionTexture) * ARRAYSIZE(square);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { square, 0, 0 };

	device->CreateBuffer(&bd, &srd, buffer);
}

void XModelMesh::CreateColoredSquare(ID3D11Device* device, ID3D11Buffer** buffer, float drawX, float drawY, int width, int height, XMFLOAT3 color)
{
	float a = 1.0F / 400.0F; //0.002500 //Half Screen Width.  //Converted To Texel Value.
	float b = 1.0F / 300.0F; //0.003333 //Half Screen Height. //Converted To Texel Value.

	float nX = -1.0F + (drawX * a);
	float nY = 1.0F - ((drawY + height) * b);

	float pX = -1.0F + ((drawX + width) * a);
	float pY = 1.0F - (drawY * b);

	VertexPositionColor square[6]
	{
		{ nX,  pY, 0.0f,	color.x, color.y, color.z}, 
		{ pX,  nY, 0.0f,	color.x, color.y, color.z}, 
		{ nX,  nY, 0.0f,	color.x, color.y, color.z}, 

		{ pX,  nY, 0.0f,	color.x, color.y, color.z}, 
		{ nX,  pY, 0.0f,	color.x, color.y, color.z},
		{ pX,  pY, 0.0f,	color.x, color.y, color.z},
	};

	// create the vertex buffer
	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VertexPositionColor) * ARRAYSIZE(square);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { square, 0, 0 };

	device->CreateBuffer(&bd, &srd, buffer);
}

void XModelMesh::CreateCubeObject(ID3D11Device* device, float xPos, float yPos, float zPos)
{
	// create vertices to represent the corners of the Hypercraft
	VertexPositionTexture OurVertices[] =
	{
		{xPos + -1.0f, yPos + -1.0f, zPos + 1.0f,    0.0f, 0.0f},    // side 1
		{xPos + 1.0f, yPos + -1.0f, zPos + 1.0f,     0.0f, 1.0f},
		{xPos + -1.0f, yPos + 1.0f, zPos + 1.0f,    1.0f, 0.0f},
		{xPos + 1.0f, yPos + 1.0f, zPos + 1.0f,       1.0f, 1.0f},

		{xPos + -1.0f, yPos + -1.0f, zPos + -1.0f,   0.0f, 0.0f},    // side 2
		{xPos + -1.0f, yPos + 1.0f, zPos + -1.0f,    0.0f, 1.0f},
		{xPos + 1.0f, yPos + -1.0f, zPos + -1.0f,   1.0f, 0.0f},
		{xPos + 1.0f, yPos + 1.0f, zPos + -1.0f,     1.0f, 1.0f},

		{xPos + -1.0f, yPos + 1.0f, zPos + -1.0f,     0.0f, 0.0f},    // side 3
		{xPos + -1.0f, yPos + 1.0f, zPos + 1.0f,      0.0f, 1.0f},
		{xPos + 1.0f, yPos + 1.0f, zPos + -1.0f,      1.0f, 0.0f},
		{xPos + 1.0f, yPos + 1.0f, zPos + 1.0f,    1.0f, 1.0f},

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
