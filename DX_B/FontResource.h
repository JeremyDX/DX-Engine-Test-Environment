#include <iostream>
#include "VertexStructureTypes.h"

class FontResource
{
	public:
		void CreateGlyphMapping();
		void AddStringToBuffer(D3D11_VIEWPORT view, const wchar_t * text, int drawX, int drawY);

		void PushStringsToDeviceBuffer(ID3D11Device * device);
		void PushStringsToDeviceBuffer(ID3D11Device * device, ID3D11Buffer ** vertexbuffer);
		int GetSize();

	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		signed __int16 glyph_positions[96];

	private:
		VertexPositionTexture vertexstorage[1024]; //Able to store 1024 character quads.
		short vertexposition = 0;
};