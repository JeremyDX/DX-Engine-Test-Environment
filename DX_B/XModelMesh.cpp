#include "pch.h"

#include "XModelMesh.h"
#include "ScreenManager.h"

/*
* Loads a Full Size Texture.
*/
__int32 XModelMesh::CreateTexturedSquare(Vertex32Byte *verts, int offset, Float3 Col, int texture_width, int texture_height, int drawX, int drawY)
{
	float offsetX = (2 * drawX) / ScreenManager::PREFERRED_CANVAS_WIDTH;
	float offsetY = (2 * drawY) / ScreenManager::PREFERRED_CANVAS_HEIGHT;

	float width = (2 * texture_width) / ScreenManager::PREFERRED_CANVAS_WIDTH;
	float height = (2 * texture_height) / ScreenManager::PREFERRED_CANVAS_HEIGHT;

	float top = 1.0F - offsetY;
	float bottom = 1.0f - height - offsetY;

	float left = -1.0F + offsetX;
	float right = -1.0f + width + offsetX;

	verts[offset++] = { left, top, 0.0f, Col._1, Col._2, Col._3, 	0.0F, 0.0F }; //Top Left.
	verts[offset++] = { right, bottom, 0.0f, Col._1, Col._2, Col._3, 	1.0F, 1.0F }; //Bottom Right.
	verts[offset++] = { left, bottom, 0.0f, Col._1, Col._2, Col._3, 	0.0F, 1.0F }; //Bottom Left.

	verts[offset++] = { right, bottom, 0.0f, Col._1, Col._2, Col._3, 	1.0F, 1.0F }; //Bottom Right.
	verts[offset++] = { left,  top, 0.0f, Col._1, Col._2, Col._3, 	0.0F, 0.0F }; //Top Left.
	verts[offset++] = { right,  top, 0.0f, Col._1, Col._2, Col._3, 	1.0F, 0.0F }; //Top Right.

	return offset;
}

void XModelMesh::CreateCubeObject(ID3D11Device* device, float xPos, float yPos, float zPos)
{
	/*// create vertices to represent the corners of the Hypercraft
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
	*/
}
