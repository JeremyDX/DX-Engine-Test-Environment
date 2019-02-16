#include "pch.h"

#include "XModelMesh.h"
#include "Landscape.h"
#include "ScreenManager.h"
#include "ContentLoader.h"

static const int GRID_SIZE = 96 * 96; //9216;
static const int MAX_COLLIDERS = 1000;
static const int COLLISON_PADDING = 20;
static const int OBJECT_ACCESS_PADDING = 40;
static int ID = 0;

struct ObjectDefinition
{
	Float2 blocking_value[4];
	uint16_t rotation;
	uint16_t width;
	uint16_t depth;
	uint16_t buffer_begin = 0;
	uint16_t buffer_end = 0;
	uint8_t id = 0;
};

struct CollisionIndexList
{
	uint16_t *ids = NULL;
	uint8_t size;

	void Insert(uint16_t id)
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

	bool InsertIfNotContained(uint16_t id)
	{
		if (ids == NULL)
		{
			size = 1;
			ids = new uint16_t[1]{ id };
			return true;
		}

		for (int i = 0; i < size; ++i)
		{
			if (ids[i] == id)
				return false;
		}

		if ((size & (size - 1)) == 0)
		{
			uint16_t *tmp = new uint16_t[size << 1];
			memcpy(tmp, ids, size * sizeof(uint16_t));
			delete[] ids;
			ids = tmp;
		}

		ids[size++] = id;
		return true;
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

int XModelMesh::CheckBasicCollision(Float3 &ref, Float2 &move_vector)
{
	int beginX = (int)(ref._1 - 1);
	int beginZ = (int)(ref._3 - 1);

	float verify_point_x = (ref._1 + move_vector._1);
	float verify_point_z = (ref._3 + move_vector._2);

	for (int x = beginX; x < beginX + 3; ++x)
	{
		for (int z = beginZ; z < beginZ + 3; ++z)
		{
			if (x < 0 || z < 0 || x > 95 || z > 95)
				continue; //Continue if bounds aren't supported for checking.

			int index = x + (z * 96);
			CollisionIndexList &c = collision_list[index];

			if (c.ids == NULL)
				continue;

			for (int i = 0; i < c.size; ++i)
			{
				ColliderChecks &collider = colliders[c.ids[i]];
				ObjectDefinition &obj = definitions[collider.object_id];

				float left_x = (collider.x_offset - obj.width - COLLISON_PADDING) / 80.0f;
				float bottom_z = (collider.z_offset - obj.depth - COLLISON_PADDING) / 80.0f;
				float right_x = (collider.x_offset + obj.width + COLLISON_PADDING) / 80.0f;
				float top_z = (collider.z_offset + obj.depth + COLLISON_PADDING) / 80.0f;

				if (verify_point_x >= left_x && verify_point_x <= right_x &&
					verify_point_z >= bottom_z && verify_point_z <= top_z)
				{

					float a = left_x;
					float b = top_z;
					float c = left_x;
					float d = bottom_z;

					bool intersected_left =
						(((a - ref._1)*(verify_point_z - ref._3) - (b - ref._3)*(verify_point_x - ref._1))
							* ((c - ref._1)*(verify_point_z - ref._3) - (d - ref._3)*(verify_point_x - ref._1)) < 0)
						&& (((ref._1 - a)*(d - b) - (ref._3 - b)*(c - a)) * ((verify_point_x - a)*(d - b) - (verify_point_z - b)*(c - a)) < 0);

					if (intersected_left)
					{
						ref._3 = verify_point_z;
						return 2;
					}
					a = left_x;
					b = bottom_z;
					c = right_x;
					d = bottom_z;

					bool intersected_bottom =
						(((a - ref._1)*(verify_point_z - ref._3) - (b - ref._3)*(verify_point_x - ref._1))
							* ((c - ref._1)*(verify_point_z - ref._3) - (d - ref._3)*(verify_point_x - ref._1)) < 0)
						&& (((ref._1 - a)*(d - b) - (ref._3 - b)*(c - a)) * ((verify_point_x - a)*(d - b) - (verify_point_z - b)*(c - a)) < 0);

					if (intersected_bottom)
					{
						ref._1 = verify_point_x;
						return 3;
					}
					a = right_x;
					b = top_z;
					c = right_x;
					d = bottom_z;

					bool intersected_right =
						(((a - ref._1)*(verify_point_z - ref._3) - (b - ref._3)*(verify_point_x - ref._1))
							* ((c - ref._1)*(verify_point_z - ref._3) - (d - ref._3)*(verify_point_x - ref._1)) < 0)
						&& (((ref._1 - a)*(d - b) - (ref._3 - b)*(c - a)) * ((verify_point_x - a)*(d - b) - (verify_point_z - b)*(c - a)) < 0);

					if (intersected_right)
					{
						ref._3 = verify_point_z;
						return 4;
					}
					a = left_x;
					b = top_z;
					c = right_x;
					d = top_z;

					bool intersected_top =
						(((a - ref._1)*(verify_point_z - ref._3) - (b - ref._3)*(verify_point_x - ref._1))
							* ((c - ref._1)*(verify_point_z - ref._3) - (d - ref._3)*(verify_point_x - ref._1)) < 0)
						&& (((ref._1 - a)*(d - b) - (ref._3 - b)*(c - a)) * ((verify_point_x - a)*(d - b) - (verify_point_z - b)*(c - a)) < 0);

					if (intersected_top)
					{
						ref._1 = verify_point_x;
						return 1;
					}
				}
			}
		}
	}
	ref._1 += move_vector._1;
	ref._3 += move_vector._2;
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

	definitions[object_id].buffer_begin = begin;
	definitions[object_id].buffer_end = buffer_index;
	definitions[object_id].id = object_id;

	definitions[object_id].rotation = 0;
	definitions[object_id].width = 40;
	definitions[object_id].depth = 40;
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

	int x_min = xunits - obj.width;
	int x_max = xunits + obj.width;
	int z_min = zunits - obj.depth;
	int z_max = zunits + obj.depth;

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

		int pot_x_start = (x_min - OBJECT_ACCESS_PADDING) / 80;
		int pot_x_end = (x_max + OBJECT_ACCESS_PADDING) / 80;
		int pot_z_start = (z_min - OBJECT_ACCESS_PADDING) / 80;
		int pot_z_end = (z_max + OBJECT_ACCESS_PADDING) / 80;

		if (pot_x_start < 0)
			pot_x_start = 0;
		if (pot_x_end > 95)
			pot_x_end = 95;
		if (pot_z_start < 0)
			pot_z_start = 0;
		if (pot_z_end > 95)
			pot_x_end = 95;

		int col_width_pad = (pot_x_end - pot_x_start) + 1;
		int col_depth_pad = (pot_z_end - pot_z_start) + 1;

		colliders[ID].object_id = id;
		colliders[ID].height_offset = 0;
		colliders[ID].x_offset = xunits;
		colliders[ID].z_offset = zunits;

		if (col_width_pad <= 3 && col_depth_pad <= 3)
		{
			int x = pot_x_start + (col_width_pad == 1 ? 0 : 1);
			int z = pot_z_start + (col_depth_pad == 1 ? 0 : 1);
			int index = x + (z * 96);
			collision_list[index].InsertIfNotContained(ID);
		} 
		else if (col_width_pad >= 4 && col_depth_pad >= 4)
		{
			if (col_width_pad <= 6 && col_depth_pad <= 6)
			{
				int cornerA = (pot_x_start + 1) + ((pot_z_start + 1) * 96);
				int cornerB = (pot_x_start + 1) + ((pot_z_end - 1) * 96);
				int cornerC = (pot_x_end - 1) + ((pot_z_start + 1) * 96);
				int cornerD = (pot_x_end - 1) + ((pot_z_end - 1) * 96);
				collision_list[cornerA].InsertIfNotContained(ID);
				collision_list[cornerB].InsertIfNotContained(ID);
				collision_list[cornerC].InsertIfNotContained(ID);
				collision_list[cornerD].InsertIfNotContained(ID);
			} 
			else
			{
				//Unsupported beyond 6x6 depth pads.
			}
		}
		else 
		{
			if (col_width_pad > col_depth_pad)
			{
				int z = pot_z_start + (col_depth_pad == 1 ? 0 : 1);
				int x = pot_x_start;
				for ( ; x < pot_x_end; x += 3)
				{
					int index = x + (z * 96);
					collision_list[index].InsertIfNotContained(ID);
				}
				x += 2;
				if (x < pot_x_end)
				{
					int index = x + (z * 96);
					collision_list[index].InsertIfNotContained(ID);
				}
			}
			else 
			{
				int x = pot_x_start + (col_width_pad == 1 ? 0 : 1);
				int z = pot_z_start;
				for ( ; z < pot_z_end; z += 3)
				{
					int index = x + (z * 96);
					collision_list[index].InsertIfNotContained(ID);
				}
				z += 2;
				if (z < pot_z_end)
				{
					int index = x + (z * 96);
					collision_list[index].InsertIfNotContained(ID);
				}
			}
		}

		++ID;
	}

	return offset;
}


