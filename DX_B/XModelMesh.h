#pragma once

#include "Constants.h"

class XModelMesh
{
	public:
		static     void LoadCollisionData();
		static     int CheckBasicCollision(Float3 & ref, Float2 & move_vector, int view_rotation, Int3 & viewport);
		static     void TestValues();
		static     void LoadObjectDefintions();
		static __int32 CreateTexturedSquare(Vertex32Byte * verts, int offset, Float3 Col, int texture_width, int texture_height, int drawX, int drawY);
		static __int32 InsertObjectToMap(Vertex32Byte * verts, int & offset, int id, int xunits, int yunits, int zunits);
};