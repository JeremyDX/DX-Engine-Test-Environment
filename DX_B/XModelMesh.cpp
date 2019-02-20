#include "pch.h"

#include "XModelMesh.h"
#include "Landscape.h"
#include "ScreenManager.h"
#include "ContentLoader.h"

const static double ONE_DEGREE_AS_RADIANS = 0.01745329251;
const static double RADIANS_TO_BIG_DEGREE = 558.50536032;

static const int GRID_SIZE = 96 * 96; //9216;
static const int MAX_COLLIDERS = 1000;
static const int COLLISON_PADDING = 20;
static const int PROMPT_PADDING = 50;
static const int OBJECT_ACCESS_PADDING = 60;
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

int XModelMesh::CheckBasicCollision(Float3 &ref, Float2 &move_vector, int view_rotation, Int3 &viewport)
{
	int beginX = (int)(ref._1 - 1);
	int beginZ = (int)(ref._3 - 1);

	float StartX = ref._1;
	float StartY = ref._3;
	float EndX = (ref._1 + move_vector._1);
	float EndY = (ref._3 + move_vector._2);

	int move_angle = (int)((atan2(move_vector._1, move_vector._2) / ONE_DEGREE_AS_RADIANS) * 32000);
	if (move_angle < 0)
		move_angle += 11520000;
	if (move_angle >= 11520000)
		move_angle -= 11520000;

	//viewport._1 = 0;

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

				float left = (collider.x_offset - obj.width - COLLISON_PADDING) * 0.0125F;
				float bottom = (collider.z_offset - obj.depth - COLLISON_PADDING) * 0.0125F;
				float right = (collider.x_offset + obj.width + COLLISON_PADDING) * 0.0125F;
				float top = (collider.z_offset + obj.depth + COLLISON_PADDING) * 0.0125F;

				if (move_angle < 5760000)
				{
					if (move_angle < 2880000) 
					{
						//Checks if we are moving in the correct direction if not then theres no way we can collide with this object.
						if (!((StartX <= left && StartY <= top) || (StartY <= bottom && StartX <= right)))
							continue;

						float MoveDifX = EndX - StartX;
						float MoveDifY = EndY - StartY;

						float LeftDifX = left - left;
						float LeftDifY = top - bottom;

						float s = (-MoveDifY * (StartX - left) + MoveDifX * (StartY - bottom));
						float t = (LeftDifX * (StartY - bottom) - LeftDifY * (StartX - left));
						float c = (-LeftDifX * MoveDifY + MoveDifX * LeftDifY);

						if (s >= 0 && s <= c && t >= 0 && t <= c)
						{
							move_vector._1 = left - StartX;
							return 0;
						}
						else 
						{
							float BottomDirX = left - right;
							float BottomDirY = bottom - bottom;

							s = (-MoveDifY * (StartX - right) + MoveDifX * (StartY - bottom));
							t = (BottomDirX * (StartY - bottom) - BottomDirY * (StartX - right));
							c = (-BottomDirX * MoveDifY + MoveDifX * BottomDirY);

							if (s >= 0 && s <= c && t >= 0 && t <= c)
							{
								move_vector._2 = bottom - StartY;
								return 0;
							}
						}

						//0-89 Degrees.
						//Left,Bottom.
					}
					else
					{
						if (!((StartX <= left && StartY <= top) || (StartY >= top && StartX <= right)))
							continue;

						float MoveDifX = EndX - StartX;
						float MoveDifY = EndY - StartY;

						float LeftDifX = left - left;
						float LeftDifY = top - bottom;

						float s = (-MoveDifY * (StartX - left) + MoveDifX * (StartY - bottom));
						float t = (LeftDifX * (StartY - bottom) - LeftDifY * (StartX - left));
						float c = (-LeftDifX * MoveDifY + MoveDifX * LeftDifY);

						if (s >= 0 && s <= c && t >= 0 && t <= c)
						{
							move_vector._1 = left - StartX;
							return 0;
						}
						else 
						{
							float TopDirX = right - left;
							float TopDirY = top - top;

							s = (-MoveDifY * (StartX - left) + MoveDifX * (StartY - top));
							t = (TopDirX * (StartY - top) - TopDirY * (StartX -left));
							c = (-TopDirX * MoveDifY + MoveDifX * TopDirY);

							if (s >= 0 && s <= c && t >= 0 && t <= c)
							{
								move_vector._2 = top - StartY;
								return 0;
							}
						}
						//90-179 Degrees.
						//Left, Top.
					}
				} else {
					if (move_angle < 8640000)
					{
						if (!((StartX >= right && StartY >= bottom) || (StartY >= top && StartX >= left)))
							continue;

						float MoveDifX = EndX - StartX;
						float MoveDifY = EndY - StartY;

						float RightDifX = right - right;
						float RightDifY = bottom - top;

						float s = (-MoveDifY * (StartX - right) + MoveDifX * (StartY - top));
						float t = (RightDifX * (StartY - top) - RightDifY * (StartX - right));
						float c = (-RightDifX * MoveDifY + MoveDifX * RightDifY);

						if (s >= 0 && s <= c && t >= 0 && t <= c)
						{
							move_vector._1 = right - StartX;
							return 0;
						}
						else
						{
							float TopDirX = right - left;
							float TopDirY = top - top;

							s = (-MoveDifY * (StartX - left) + MoveDifX * (StartY - top));
							t = (TopDirX * (StartY - top) - TopDirY * (StartX - left));
							c = (-TopDirX * MoveDifY + MoveDifX * TopDirY);

							if (s >= 0 && s <= c && t >= 0 && t <= c)
							{
								move_vector._2 = top - StartY;
								return 0;
							}
						}
						//180-269 Degrees.
						//Right,Top
					}
					else 
					{

						if (!((StartX >= right && StartY >= bottom) || (StartY <= bottom && StartX >= left)))
							continue;

						float MoveDifX = EndX - StartX;
						float MoveDifY = EndY - StartY;

						float RightDifX = right - right;
						float RightDifY = bottom - top;

						float s = (-MoveDifY * (StartX - right) + MoveDifX * (StartY - top));
						float t = (RightDifX * (StartY - top) - RightDifY * (StartX - right));
						float c = (-RightDifX * MoveDifY + MoveDifX * RightDifY);

						if (s >= 0 && s <= c && t >= 0 && t <= c)
						{
							move_vector._1 = right - StartX;
							return 0;
						}
						else {
							float BottomDirX = left - right;
							float BottomDirY = bottom - bottom;

							s = (-MoveDifY * (StartX - right) + MoveDifX * (StartY - bottom));
							t = (BottomDirX * (StartY - bottom) - BottomDirY * (StartX - right));
							c = (-BottomDirX * MoveDifY + MoveDifX * BottomDirY);

							if (s >= 0 && s <= c && t >= 0 && t <= c)
							{
								move_vector._2 = bottom - StartY;
								return 0;
							}
						}
						//270-359 Degrees.
						//Right, Bottom.
					}
				}
					/*
				if (check_collison)
				{

				}

				//Action Prompt Checks.
				if (viewport._1 == -1)
				{
					int radial_width = (obj.width + PROMPT_PADDING) * (obj.width + PROMPT_PADDING);
					int radial_depth = (obj.depth + PROMPT_PADDING) * (obj.depth + PROMPT_PADDING);

					int dx = (collider.x_offset - StartX);
					int dy = (collider.z_offset - StartY);

					if (1.0f >= (((dx * dx) / (float)radial_width)) + (((dy * dy) / (float)radial_depth)))
					{
						bool onleft_right = bottom <= StartY && top >= StartY;
						bool ontop_bottom = left <= StartX && right >= StartX;

						if (ontop_bottom || onleft_right)
						{

							if (onleft_right)
							{
								bool left = right >= StartX;
								if (left)
								{
									viewport._1 = 1;
									viewport._2 = 10 * 32000;
									viewport._3 = 170 * 32000;
								}
								else
								{
									viewport._1 = 3;
									viewport._2 = 190 * 32000;
									viewport._3 = 350 * 32000;
								}
							}
							else
							{
								bool bottom = top >= StartY;
								if (bottom)
								{
									viewport._1 = 2;
									viewport._2 = 280 * 32000;
									viewport._3 = 80 * 32000;
								}
								else
								{
									viewport._1 = 0;
									viewport._2 = 100 * 32000;
									viewport._3 = 260 * 32000;
								}
							}
						}
					}
				}
			*/
			}
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
	Float3 c = { ContentLoader::CreateShaderColor(0.5f, 1.0f), 0.5f, 0.5f };
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
				int junk3 = 0;
				//Unsupported beyond 6x6 depth pads.
			}
		}
		else 
		{
			if (col_width_pad <= 5 && col_depth_pad <= 5)
			{
				if (col_width_pad > col_depth_pad)
				{
					int zz = pot_z_start;
					if (col_depth_pad > 1)
						zz += 1;

					int index = pot_x_start + (zz * 96);
					int index2 = pot_x_end + (zz * 96);

					collision_list[index].InsertIfNotContained(ID);
					collision_list[index2].InsertIfNotContained(ID);
				}
				else
				{
					int xx = pot_x_start;
					if (col_depth_pad > 1)
						xx += 1;

					int index = xx + (pot_z_start * 96);
					int index2 = xx + (pot_z_end * 96);

					collision_list[index].InsertIfNotContained(ID);
					collision_list[index2].InsertIfNotContained(ID);
				}
			}
			else {
				int junk2 = 0;
				//Unsupported Padding Beyond (1-3)x(4-5) Sized Objects.
			}
		}

		++ID;
	}

	return offset;
}


