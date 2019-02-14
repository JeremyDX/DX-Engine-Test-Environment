#include "pch.h"

#include "XModelMesh.h"
#include "Landscape.h"
#include "ScreenManager.h"
#include "ContentLoader.h"

static const int GRID_SIZE = 96 * 96; //7680;
static const int MAX_COLLIDERS = 1000;
static int ID = 0;

struct ObjectDefinition
{
	Float2 blocking_value[4];
	uint16_t rotation;
	uint16_t radius;
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


static ObjectDefinition *definitions;

CollisionIndexList *collision_list;
ColliderChecks *colliders;

void XModelMesh::LoadCollisionData()
{
	collision_list = new CollisionIndexList[GRID_SIZE];
	colliders = new ColliderChecks[MAX_COLLIDERS];
}

bool XModelMesh::VerifyCollisionPlacement(Float3 &ref, Float2 block[])
{
	CollisionIndexList &c = collision_list[((int)(ref._1)) + (((int)ref._3) * 96)];
	if (c.ids == NULL)
		return true; //This tile wasn't handled so it is safe to walk on. 

	if (c.size == 0)
	{
		//No Colliders because this tile is completely unwalkable from all sides of entry.
	}

	int pX = ref._1 * 80;
	int pZ = ref._3 * 80;

	for (int i = 0; i < c.size; ++i)
	{
		ColliderChecks &collider = colliders[c.ids[i]];
		ObjectDefinition &obj = definitions[collider.object_id];
		int x = collider.x_offset;
		int z = collider.z_offset;
		int radius = definitions[collider.object_id].radius + 800;

		int xDiff = pX - x;
		int yDiff = pZ - z;
		int absX = xDiff * xDiff;
		int absY = yDiff * yDiff;

		if (radius >= (absX + absY))
		{
			return false;
		}
	}
	return true;
}

bool XModelMesh::CheckTurnCollision(Float3 &ref, Float2 block[], uint32_t last_rot, uint32_t current_rot, uint8_t quadrant)
{
	CollisionIndexList &c = collision_list[((int)(ref._1)) + (((int)ref._3) * 96)];
	if (c.ids == NULL)
		return true;

	int pX = ref._1 * 80;
	int pZ = ref._3 * 80;

	uint32_t difference = current_rot - last_rot; //Positive = Right Turn, Negative = Left Turn.


	for (int i = 0; i < c.size; ++i)
	{
		ColliderChecks &collider = colliders[c.ids[i]];
		ObjectDefinition &obj = definitions[collider.object_id];
		int x = collider.x_offset;
		int z = collider.z_offset;
		int radius = definitions[collider.object_id].radius + 800;

		int xDiff = pX - x;
		int yDiff = pZ - z;
		int absX = xDiff * xDiff;
		int absY = yDiff * yDiff;

		if (radius >= (absX + absY))
		{
			int TR = (0 - quadrant) & 0x3;
			int TL = (1 - quadrant) & 0x3;
			int BL = (2 - quadrant) & 0x3;
			int BR = (3 - quadrant) & 0x3;

			if (difference < 0)
				//TURNING LEFT.
			{


				//CheckLines(obj.blocking_value, block, 1, 0);
			}
			else 
				//TURNING RIGHT. 
			{

			}
			return false;
		}
	}
	return true;
}

uint16_t XModelMesh::GetColliderListBeginIndex(Float3 &ref, Float2 block[], uint16_t my_rot, uint8_t size_offset)
{
	CollisionIndexList &c = collision_list[((int)(ref._1)) + (((int)ref._3) * 96)];
	if (c.ids == NULL)
		return 0;

	int pX = ref._1 * 80;
	int pZ = ref._3 * 80;

	for (int i = 0; i < c.size; ++i)
	{
		ColliderChecks &collider = colliders[c.ids[i]];
		ObjectDefinition &obj = definitions[collider.object_id];
		int x = collider.x_offset;
		int z = collider.z_offset;
		int radius = definitions[collider.object_id].radius + 800;

		int rX = (pX - x) * (pX - x);
		int rY = (pZ - z) * (pZ - z);

		if (radius >= (rX + rY))
		{
			//We found a checkable collider.

			float ox = x / 80.0F;
			float oz = z / 80.0f;

			obj.blocking_value[0]._1 += ox;
			obj.blocking_value[0]._2 += oz;

			obj.blocking_value[1]._1 += ox;
			obj.blocking_value[1]._2 += oz;

			obj.blocking_value[2]._1 += ox;
			obj.blocking_value[2]._2 += oz;

			obj.blocking_value[3]._1 += ox;
			obj.blocking_value[3]._2 += oz;


			return 1;
		}
	}
	return 0;
}

void XModelMesh::TestValues()
{

}

void XModelMesh::LoadObjectDefintions()
{
	definitions = new ObjectDefinition[1];

	int object_id = 0;
	int buffer_index = -1;
	int begin = 0;

	object_buffer[++buffer_index] = { -0.5F, -0.1, 0.5F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.5F, 0.1, 0.5F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.5F, 0.1, -0.5F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { -0.5F, 0.1, -0.5F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.5F, -0.1, -0.5F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.5F, -0.1, 0.5F, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { -0.5F, -0.1, -0.5F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.5F, 0.1, -0.5F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.5F, 0.1, -0.5F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { 0.5F, 0.1, -0.5F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.5F, -0.1, -0.5F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.5F, -0.1, -0.5F, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { 0.5F, -0.1, -0.5F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.5F, 0.1, -0.5F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.5F, 0.1, 0.5F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { 0.5F, 0.1, 0.5F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.5F, -0.1, 0.5F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.5F, -0.1, -0.5F, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { 0.5F, -0.1, 0.5F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.5F, 0.1, 0.5F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.5F, 0.1, 0.5F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { -0.5F, 0.1, 0.5F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.5F, -0.1, 0.5F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.5F, -0.1, 0.5F, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { -0.5F, -0.1, -0.5F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.5F, -0.1, -0.5F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { 0.5F, -0.1, 0.5F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { 0.5F, -0.1, 0.5F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.5F, -0.1, 0.5F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { -0.5F, -0.1, -0.5F, 1.0f, 1.0f };

	object_buffer[++buffer_index] = { 0.5F, 0.1, -0.5F, 1.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.5F, 0.1, -0.5F, 0.0f, 1.0f };
	object_buffer[++buffer_index] = { -0.5F, 0.1, 0.5F, 0.0f, 0.0f };

	object_buffer[++buffer_index] = { -0.5F, 0.1, 0.5F, 0.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.5F, 0.1, 0.5F, 1.0f, 0.0f };
	object_buffer[++buffer_index] = { 0.5F, 0.1, -0.5F, 1.0f, 1.0f };

	definitions[object_id].radius = 3201;

	definitions[object_id].buffer_begin = begin;
	definitions[object_id].buffer_end = buffer_index;
	definitions[object_id].id = object_id;

	definitions[object_id].rotation = 0;

	float SinFace = 0.0F, CosFace = 1.0F;

	float SinH = SinFace * 0.5F;
	float CosH = CosFace * 0.5F;
	float SinW = SinFace * 0.5F;
	float CosW = CosFace * 0.5F;

	definitions[object_id].blocking_value[0]._1 = -CosW + SinH;
	definitions[object_id].blocking_value[0]._2 =  SinW + CosH;

	definitions[object_id].blocking_value[1]._1 =  CosW + SinH;
	definitions[object_id].blocking_value[1]._2 = -SinW + CosH;

	definitions[object_id].blocking_value[2]._1 = -definitions[object_id].blocking_value[0]._1;
	definitions[object_id].blocking_value[2]._2 = -definitions[object_id].blocking_value[0]._2;

	definitions[object_id].blocking_value[3]._1 = -definitions[object_id].blocking_value[1]._1;
	definitions[object_id].blocking_value[3]._2 = -definitions[object_id].blocking_value[1]._2;
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
	float x = xunits / 80.0f;
	float y = yunits / 80.0f;
	float z = zunits / 80.0f;
	Float3 c = { ContentLoader::CreateShaderColor(0.5f, 0.8f), 1.0f, 1.0f };
	ObjectDefinition &obj = definitions[id];
	for (int i = obj.buffer_begin; i <= obj.buffer_end; ++i)
	{
		verts[++offset] = 
		{
			x + object_buffer[i]._X, 
			y + object_buffer[i]._Y, 
			z + object_buffer[i]._Z,
			c._1, 
			c._2, 
			c._3,
			object_buffer[i]._U, 
			object_buffer[i]._V
		};
	}

	int rad = sqrt(obj.radius);

	float x_min = xunits - rad;
	float x_max = xunits + rad;
	float z_min = zunits - rad;
	float z_max = zunits + rad;

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


