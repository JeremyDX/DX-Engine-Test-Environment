#pragma once

#include "VertexStructureTypes.h"
#include "TextureResource.h"
#include "FontResource.h"

using namespace Microsoft::WRL;

class XModelMesh
{
	private:
		VertexPositionTexture VertexStorage[1024];
		short IndexStorage[1024];

		int vertex_count = 0;
		int index_count = 0;

	public:
		void CreateTexturedSquare(ID3D11Device * device, ID3D11Buffer ** buffer);
		void DrawString(ID3D11Device * device, FontResource resource, std::string text, float drawX, float drawY);
		void CreateCubeObject(ID3D11Device *device, float xPos, float yPos, float zPos);

		ComPtr<ID3D11Buffer> vertexbuffer;
		ComPtr<ID3D11Buffer> indexbuffer;
};