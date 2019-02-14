#pragma once

#include "Constants.h"

class XModelMesh
{
	public:
		static     void LoadCollisionData();
		static bool VerifyCollisionPlacement(Float3 & ref, Float2 block[]);
		static bool CheckTurnCollision(Float3 & ref, Float2 block[], uint32_t last_rot, uint32_t current_rot, uint8_t quadrant);
		static uint16_t GetColliderListBeginIndex(Float3 & ref, Float2 block[], uint16_t my_rot, uint8_t offset);
		static     void TestValues();
		static     void LoadObjectDefintions();
		static __int32 CreateTexturedSquare(Vertex32Byte * verts, int offset, Float3 Col, int texture_width, int texture_height, int drawX, int drawY);
		static __int32 InsertObjectToMap(Vertex32Byte * verts, int & offset, int id, int xunits, int yunits, int zunits);
};