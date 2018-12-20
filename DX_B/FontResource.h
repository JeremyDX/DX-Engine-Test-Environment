#include "VertexStructureTypes.h"
#include "TextureResource.h"

using namespace Microsoft::WRL;

class FontResource
{
	private:
		VertexPositionTexture VertexStorage[1024];
		short IndexStorage[1024];

	public:

		ComPtr<ID3D11Buffer> vertexbuffer;
};