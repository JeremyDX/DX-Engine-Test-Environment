#pragma once

#include "VertexStructureTypes.h"
#include "TextureResource.h"
#include "FontResource.h"

using namespace Microsoft::WRL;

class XModelMesh
{
	private:
		VertexPositionColorTexture VertexStorage[1024];
		short IndexStorage[1024];

		int vertex_count = 0;
		int index_count = 0;

	public:
		void CreateTexturedSquare(ID3D11Device * device, D3D11_VIEWPORT view, ID3D11Buffer ** buffer);
		void CreateCubeObject(ID3D11Device *device, float xPos, float yPos, float zPos);

		ComPtr<ID3D11Buffer> vertexbuffer;
		ComPtr<ID3D11Buffer> indexbuffer;
};