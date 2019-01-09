#pragma once

#include "Constants.h"

class XModelMesh
{
	private:
		Vertex32Byte VertexStorage[1024];
		short IndexStorage[1024];

		int vertex_count = 0;
		int index_count = 0;

	public:
		static __int32 CreateTexturedSquare(Vertex32Byte * verts, int offset, Float3 Col, int texture_width, int texture_height, int drawX, int drawY);
		void CreateCubeObject(ID3D11Device *device, float xPos, float yPos, float zPos);

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexbuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexbuffer;
};