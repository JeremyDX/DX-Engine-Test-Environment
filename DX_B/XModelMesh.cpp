#include "pch.h"

#include "XModelMesh.h"
#include "Landscape.h"
#include "ScreenManager.h"

struct ObjectDefiniton
{
	Int3 collision_size = { 0 };
	uint16_t buffer_begin = 0;
	uint16_t buffer_end = 0;
	uint8_t id = 0;
};

Landscape landscapes[9];

static Vertex20Byte object_buffer[4096];

static ObjectDefiniton *definitions;

/*
* Once again.. hardcoded :(
*/
void XModelMesh::LoadObjectDefintions()
{
	definitions = new ObjectDefiniton[1];

	int object_id = 0;
	int buffer_index = -1;
	int begin = 0;

	object_buffer[++buffer_index] = { -0.549039, -0.1, 0.550959, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.549039, 0.1, 0.550959, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.550959, 0.1, -0.549039, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { -0.550959, 0.1, -0.549039, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.550959, -0.1, -0.549039, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.549039, -0.1, 0.550959, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { -0.550959, -0.1, -0.549039, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.550959, 0.1, -0.549039, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.549039, 0.1, -0.550959, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { 0.549039, 0.1, -0.550959, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.549039, -0.1, -0.550959, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.550959, -0.1, -0.549039, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { 0.549039, -0.1, -0.550959, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.549039, 0.1, -0.550959, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.550959, 0.1, 0.549039, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { 0.550959, 0.1, 0.549039, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.550959, -0.1, 0.549039, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.549039, -0.1, -0.550959, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { 0.550959, -0.1, 0.549039, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.550959, 0.1, 0.549039, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.549039, 0.1, 0.550959, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { -0.549039, 0.1, 0.550959, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.549039, -0.1, 0.550959, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.550959, -0.1, 0.549039, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { -0.550959, -0.1, -0.549039, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.549039, -0.1, -0.550959, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.550959, -0.1, 0.549039, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { 0.550959, -0.1, 0.549039, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.549039, -0.1, 0.550959, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.550959, -0.1, -0.549039, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { 0.549039, 0.1, -0.550959, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.550959, 0.1, -0.549039, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.549039, 0.1, 0.550959, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { -0.549039, 0.1, 0.550959, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.550959, 0.1, 0.549039, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.549039, 0.1, -0.550959, 1.0f, 1.0f };

	definitions[object_id].buffer_begin = begin;
	definitions[object_id].buffer_end = buffer_index;
	definitions[object_id].id = object_id;
}

/*
* Loads a Full Size Texture.
*/
__int32 XModelMesh::CreateTexturedSquare(Vertex32Byte *verts, int offset, Float3 Col, int texture_width, int texture_height, int drawX, int drawY)
{
	float offsetX = (2 * drawX) / (float)ScreenManager::PREFERRED_CANVAS_WIDTH;
	float offsetY = (2 * drawY) / (float)ScreenManager::PREFERRED_CANVAS_HEIGHT;

	float width = (2 * texture_width) / (float)ScreenManager::PREFERRED_CANVAS_WIDTH;
	float height = (2 * texture_height) / (float)ScreenManager::PREFERRED_CANVAS_HEIGHT;

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

__int32 XModelMesh::InsertObjectToMap(Vertex32Byte * verts, int & offset, int id, int xunits, int yunits, int zunits)
{
	float x = xunits / 100.0f;
	float y = yunits / 100.0f;
	float z = zunits / 100.0f;
	Float3 c = { 1.0f, 1.0f, 1.0f };
	ObjectDefiniton &obj = definitions[id];
	for (int i = obj.buffer_begin; i <= obj.buffer_end; ++i)
	{
		verts[++offset] = {
			object_buffer[i]._X + x, object_buffer[i]._Y + y, object_buffer[i]._Z + z,
			c._1, c._2, c._3,
			object_buffer[i]._U, object_buffer[i]._V
		};
	}
	return offset;
}


