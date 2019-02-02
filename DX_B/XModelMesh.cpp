#include "pch.h"

#include "XModelMesh.h"
#include "Landscape.h"
#include "ScreenManager.h"
#include "ContentLoader.h"

static const int GRID_SIZE = 96 * 96; //7680;
static const int MAX_COLLIDERS = 1000;
static int ID = 0;

struct ObjectDefiniton
{
	Byte3 collision = { 0 };
	uint16_t buffer_begin = 0;
	uint16_t buffer_end = 0;
	uint8_t id = 0;
};

struct CollisionIndexList
{
	uint16_t *ids = NULL;
	uint8_t size;

	void Insert(uint8_t id)
	{
		if (ids == NULL)
		{
			size = 1;
			ids = new uint16_t[1]{ id };
			return;
		}

		if ((size & (size - 1)) == 0)
		{
			uint16_t *tmp = new uint16_t[size << 1];
			memcpy(tmp, ids, size * sizeof(uint16_t));
			delete[] ids;
			ids = tmp;
		}

		ids[size++] = id;
	}
};

struct ColliderChecks
{
	//The center location of collider initial check.
	uint16_t x_offset;
	uint16_t z_offset;

	//The offset property for how far off the terrain this collider exists.
	uint16_t height_offset;

	//Used for refering this objects collision and property data.
	uint8_t object_id;
};

static Vertex20Byte object_buffer[4096];


static ObjectDefiniton *definitions;

CollisionIndexList *collision_list;
ColliderChecks *colliders;

void XModelMesh::LoadCollisionData()
{
	collision_list = new CollisionIndexList[GRID_SIZE];
	colliders = new ColliderChecks[MAX_COLLIDERS];
}

uint16_t XModelMesh::GetCurrentCollidorIndex(Float3 &ref)
{
	CollisionIndexList &c = collision_list[((int)(ref._1)) + (((int)ref._3) * 96)];
	if (c.ids == NULL)
		return 0;
	return c.size;
}

void XModelMesh::TestValues()
{

}

void XModelMesh::LoadObjectDefintions()
{
	definitions = new ObjectDefiniton[1];

	int object_id = 0;
	int buffer_index = -1;
	int begin = 0;

	object_buffer[++buffer_index] = { -0.55F, -0.1, 0.55F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.55F, 0.1, 0.55F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.55F, 0.1, -0.55F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { -0.55F, 0.1, -0.55F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.55F, -0.1, -0.55F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.55F, -0.1, 0.55F, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { -0.55F, -0.1, -0.55F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.55F, 0.1, -0.55F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.55F, 0.1, -0.55F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { 0.55F, 0.1, -0.55F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.55F, -0.1, -0.55F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.55F, -0.1, -0.55F, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { 0.55F, -0.1, -0.55F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.55F, 0.1, -0.55F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.55F, 0.1, 0.55F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { 0.55F, 0.1, 0.55F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.55F, -0.1, 0.55F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.55F, -0.1, -0.55F, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { 0.55F, -0.1, 0.55F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.55F, 0.1, 0.55F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.55F, 0.1, 0.55F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { -0.55F, 0.1, 0.55F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.55F, -0.1, 0.55F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.55F, -0.1, 0.55F, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { -0.55F, -0.1, -0.55F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.55F, -0.1, -0.55F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.55F, -0.1, 0.55F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { 0.55F, -0.1, 0.55F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.55F, -0.1, 0.55F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.55F, -0.1, -0.55F, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { 0.55F, 0.1, -0.55F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.55F, 0.1, -0.55F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.55F, 0.1, 0.55F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { -0.55F, 0.1, 0.55F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.55F, 0.1, 0.55F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.55F, 0.1, -0.55F, 1.0f, 1.0f };

	definitions[object_id].collision._1 = (uint8_t)44;
	definitions[object_id].collision._2 = (uint8_t)44;
	definitions[object_id].collision._3 = (uint8_t)sqrt(44 * 44 + 44 * 44);

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

__int32 XModelMesh::InsertObjectToMap(Vertex32Byte * verts, uint32_t * minimap, int & offset, int id, int xunits, int yunits, int zunits)
{
	float x = xunits / 80.0f;
	float y = yunits / 80.0f;
	float z = zunits / 80.0f;
	Float3 c = { ContentLoader::CreateShaderColor(0.5f, 0.8f), 1.0f, 1.0f };
	ObjectDefiniton &obj = definitions[id];
	for (int i = obj.buffer_begin; i <= obj.buffer_end; ++i)
	{
		verts[++offset] = 
		{
			object_buffer[i]._X + x, 
			object_buffer[i]._Y + y, 
			object_buffer[i]._Z + z,
			c._1, 
			c._2, 
			c._3,
			object_buffer[i]._U, 
			object_buffer[i]._V
		};
	}

	float x_min = xunits - obj.collision._1;
	float x_max = xunits + obj.collision._1;
	float z_min = zunits - obj.collision._2;
	float z_max = zunits + obj.collision._2;

	if (!(z_max < 0 || x_max < 0 || z_min >= 7680 || x_min >= 7680))
	{
		if (z_min < 0)
			z_min = 0;
		if (x_min < 0)
			x_min = 0;
		if (z_max > 7679)
			z_max = 7679;
		if (x_max > 7679)
			x_max = 7679;

		int x_start = x_min / 80;
		int x_end = x_max / 80;
		int z_start = z_min / 80;
		int z_end = z_max / 80;

		int offset = 0;

		colliders[ID].object_id = id;
		colliders[ID].height_offset = 0;
		colliders[ID].x_offset = xunits;
		colliders[ID].z_offset = zunits;

		//3.75X , 5.00 Z
		//
		//300, 400
		//280, 300

		for (int x = x_start; x <= x_end; ++x)
		{
			int index_bottom = x + (z_start * 96);
			int index_top = x + (z_end * 96);

			collision_list[index_bottom].Insert(ID);
			collision_list[index_top].Insert(ID);
		}

		for (int z = z_start + 1; z < z_end; ++z)
		{
			int index_left = x_start + (z * 96);
			int index_right = x_end + (z * 96);
		}

		++ID;
	}

	return offset;
}


