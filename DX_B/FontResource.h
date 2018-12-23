#include <iostream>
#include "VertexStructureTypes.h"

class FontResource
{
	public:
		void CreateGlyphMapping();
		void AddStringToBuffer(D3D11_VIEWPORT view, const wchar_t * text, int drawX, int drawY);

		void PushStringsToDeviceBuffer(ID3D11Device * device);
		 int GetSize();

	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

	private:
		Vertex4F glyph_mapped_uvs[96];
		VertexPositionTexture vertexstorage[1024];
		short vertexposition = 0;
};